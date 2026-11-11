// libxrtap.so: LD_PRELOAD tap for XRService camera capture.
// Inert in every process except XRService. In XRService it interposes ioctl()
// and, after a successful VIDIOC_DQBUF on a whitelisted V4L2 capture node,
// copies plane 0 of the dequeued buffer into /dev/shm/xrtap. It never changes
// the arguments or the result XRService sees, never blocks, and copies only
// while a reader is attached (reader heartbeat), so it costs nothing otherwise.
#define _GNU_SOURCE
#include "xrtap.h"
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/dma-buf.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static int (*real_ioctl)(int, unsigned long, ...);
static void *(*real_mmap)(void *, size_t, int, int, int, off_t);
static int active;        // ring mapped and copying (trackers in use)
static int is_xrservice; // this process is XRService
#ifndef XRTAP_SUBDEV_EVENTS
#define XRTAP_SUBDEV_EVENTS 0  /* 1 = record subdev control ioctls (diagnostic) */
#endif
static xrtap_shm *shm;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Upper tracking cameras by default; XRTAP_DEVICES="/dev/video0,/dev/video3,..." overrides.
static char devices[256] = "/dev/video0,/dev/video3,/dev/video9,/dev/video13";

#define MAX_FD 4096
static signed char fd_node[MAX_FD];        // -2 unknown, -1 not tapped, >=0 node index
static signed char fd_subdev[MAX_FD];      // -2 unknown, -1 not a subdev, else N of /dev/v4l-subdevN
#define MAX_MAPS 256
static struct { int fd; dev_t dev; ino_t ino; void *addr; size_t len; } maps[MAX_MAPS];   // dmabuf mappings
static struct { int fd; off_t offset; void *addr; size_t len; } mmaps[MAX_MAPS];          // V4L2 MMAP buffers

static int64_t now_ns(clockid_t id) {
    struct timespec t;
    clock_gettime(id, &t);
    return (int64_t)t.tv_sec * 1000000000ll + t.tv_nsec;
}

__attribute__((constructor)) static void xrtap_init(void) {
    real_ioctl = (int (*)(int, unsigned long, ...))dlsym(RTLD_NEXT, "ioctl");
    real_mmap = (void *(*)(void *, size_t, int, int, int, off_t))dlsym(RTLD_NEXT, "mmap");
    char exe[512];
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n <= 0) return;
    exe[n] = 0;
    const char *base = strrchr(exe, '/');
    if (strcmp(base ? base + 1 : exe, "XRService") != 0) return;
    const char *env = getenv("XRTAP_DEVICES");
    if (env && strlen(env) < sizeof devices) strcpy(devices, env);
    is_xrservice = 1;  // dormant until /dev/shm/xrtap-enable exists (trackers in use)
}

// Caller holds lock.
static void activate(void) {
    int fd = open("/dev/shm/xrtap", O_RDWR | O_CREAT | O_CLOEXEC, 0644);
    if (fd < 0) return;
    if (ftruncate(fd, sizeof(xrtap_shm)) != 0) { close(fd); return; }
    void *p = real_mmap(NULL, sizeof(xrtap_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (p == MAP_FAILED) return;
    shm = (xrtap_shm *)p;
    memset(shm, 0, sizeof(xrtap_shm));
    shm->version = XRTAP_VERSION;
    shm->writer_pid = getpid();
    memset(fd_node, -2, sizeof fd_node);
    memset(fd_subdev, -2, sizeof fd_subdev);
    __atomic_store_n(&shm->magic, XRTAP_MAGIC, __ATOMIC_RELEASE);
    active = 1;
}

// Caller holds lock. Frees the 34 MB ring when the trackers are no longer used.
static void deactivate(void) {
    if (!active) return;
    active = 0;
    munmap(shm, sizeof(xrtap_shm));
    shm = NULL;
    unlink("/dev/shm/xrtap");
}

// Checks the enable flag at most every 2 s; cheap enough to call on every DQBUF.
static void poll_enable(void) {
    static int64_t next_check;
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    const int64_t now = (int64_t)t.tv_sec * 1000000000ll + t.tv_nsec;
    if (now < __atomic_load_n(&next_check, __ATOMIC_RELAXED)) return;
    __atomic_store_n(&next_check, now + 2000000000ll, __ATOMIC_RELAXED);
    const int want = access(XRTAP_ENABLE_FLAG, F_OK) == 0;
    if (want == active) return;
    if (pthread_mutex_lock(&lock) != 0) return;
    if (want) activate(); else deactivate();
    pthread_mutex_unlock(&lock);
}

static int whitelisted(const char *path) {
    size_t len = strlen(path);
    for (const char *s = devices; *s;) {
        const char *e = strchr(s, ',');
        size_t n = e ? (size_t)(e - s) : strlen(s);
        if (n == len && !strncmp(s, path, n)) return 1;
        if (!e) break;
        s = e + 1;
    }
    return 0;
}

// Caller holds lock.
static int node_for_fd(int fd) {
    if (fd < 0 || fd >= MAX_FD) return -1;
    if (fd_node[fd] != -2) return fd_node[fd];
    char link[64], path[64] = {0};
    snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
    ssize_t n = readlink(link, path, sizeof path - 1);
    int node = -1;
    if (n > 0 && whitelisted(path)) {
        for (uint32_t i = 0; i < shm->nodes_used; ++i)
            if (!strcmp(shm->nodes[i].device, path)) node = (int)i;
        if (node < 0 && shm->nodes_used < XRTAP_NODES) {
            node = (int)shm->nodes_used;
            strncpy(shm->nodes[node].device, path, sizeof shm->nodes[node].device - 1);
            __atomic_store_n(&shm->nodes_used, shm->nodes_used + 1, __ATOMIC_RELEASE);
        }
    }
    fd_node[fd] = (signed char)node;
    return node;
}

static void *dmabuf_map(int fd, size_t need, size_t *len) {
    struct stat st;
    if (fstat(fd, &st) != 0) return NULL;
    for (int i = 0; i < MAX_MAPS; ++i)
        if (maps[i].addr && maps[i].fd == fd && maps[i].dev == st.st_dev && maps[i].ino == st.st_ino && maps[i].len >= need) {
            *len = maps[i].len;
            return maps[i].addr;
        }
    size_t size = (size_t)lseek(fd, 0, SEEK_END);
    if (size == (size_t)-1 || size < need) size = need;
    void *p = real_mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) return NULL;
    for (int i = 0; i < MAX_MAPS; ++i)
        if (!maps[i].addr || maps[i].fd == fd) {
            if (maps[i].addr) munmap(maps[i].addr, maps[i].len);
            maps[i].fd = fd; maps[i].dev = st.st_dev; maps[i].ino = st.st_ino; maps[i].addr = p; maps[i].len = size;
            *len = size;
            return p;
        }
    munmap(p, size);
    return NULL;
}

static void refresh_format(int fd, xrtap_node *node, unsigned type) {
    struct v4l2_format f;
    memset(&f, 0, sizeof f);
    f.type = type;
    if (real_ioctl(fd, VIDIOC_G_FMT, &f) != 0) return;
    if (type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
        node->width = f.fmt.pix_mp.width; node->height = f.fmt.pix_mp.height;
        node->stride = f.fmt.pix_mp.plane_fmt[0].bytesperline; node->pixelformat = f.fmt.pix_mp.pixelformat;
        // CAMSS reports bytesperline=1056 for 1056-wide NV12 but lays the Y plane out with the aligned
        // stride implied by sizeimage (1769472 = 1152 * 1024 * 3/2, verified by image content).
        if (node->pixelformat == V4L2_PIX_FMT_NV12 && node->height) {
            const uint32_t aligned = (uint32_t)((uint64_t)f.fmt.pix_mp.plane_fmt[0].sizeimage * 2 / 3 / node->height);
            if (aligned > node->stride) node->stride = aligned;
        }
    } else {
        node->width = f.fmt.pix.width; node->height = f.fmt.pix.height;
        node->stride = f.fmt.pix.bytesperline; node->pixelformat = f.fmt.pix.pixelformat;
    }
}

static void tap(int fd, const struct v4l2_buffer *b, int64_t mono, int64_t raw) {
    if (pthread_mutex_lock(&lock) != 0) return;
    if (!active) { pthread_mutex_unlock(&lock); return; }  // disabled since the check
    int64_t beat = __atomic_load_n(&shm->reader_heartbeat_ns, __ATOMIC_ACQUIRE);
    int index = node_for_fd(fd);
    if (index < 0) { pthread_mutex_unlock(&lock); return; }
    xrtap_node *node = &shm->nodes[index];
    if (!beat || mono - beat > XRTAP_READER_WINDOW_NS) { node->skipped++; pthread_mutex_unlock(&lock); return; }
    if (!node->width) refresh_format(fd, node, b->type);
    const int mplane = b->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    uint32_t stride = node->stride ? node->stride : node->width;
    uint64_t bytes64 = (uint64_t)stride * node->height;
    const uint8_t *src = NULL;
    size_t maplen = 0;
    int dmabuf = -1;
    if (b->memory == V4L2_MEMORY_DMABUF) {
        dmabuf = mplane ? (b->m.planes && b->length ? b->m.planes[0].m.fd : -1) : b->m.fd;
        uint32_t offset = mplane && b->m.planes ? b->m.planes[0].data_offset : 0;
        if (dmabuf >= 0) {
            const uint8_t *base = (const uint8_t *)dmabuf_map(dmabuf, offset + bytes64, &maplen);
            if (base) src = base + offset;
        }
    } else if (b->memory == V4L2_MEMORY_MMAP) {
        off_t offset = mplane ? (b->m.planes && b->length ? (off_t)b->m.planes[0].m.mem_offset : -1) : (off_t)b->m.offset;
        for (int i = 0; i < MAX_MAPS; ++i)
            if (mmaps[i].addr && mmaps[i].fd == fd && mmaps[i].offset == offset && mmaps[i].len >= bytes64) src = (const uint8_t *)mmaps[i].addr;
    }
    if (!src || !node->height || bytes64 > XRTAP_MAX_BYTES) { node->skipped++; pthread_mutex_unlock(&lock); return; }
    uint64_t number = node->write_seq;
    xrtap_slot *slot = &node->slots[number % XRTAP_SLOTS];
    __atomic_store_n(&slot->seq, slot->seq + 1, __ATOMIC_RELEASE);  // odd: writing
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    if (dmabuf >= 0) { struct dma_buf_sync s = { DMA_BUF_SYNC_START | DMA_BUF_SYNC_READ }; real_ioctl(dmabuf, DMA_BUF_IOCTL_SYNC, &s); }
    memcpy(slot->pixels, src, (size_t)bytes64);
    if (dmabuf >= 0) { struct dma_buf_sync s = { DMA_BUF_SYNC_END | DMA_BUF_SYNC_READ }; real_ioctl(dmabuf, DMA_BUF_IOCTL_SYNC, &s); }
    slot->frame_number = number;
    slot->v4l2_sequence = b->sequence;
    slot->v4l2_flags = b->flags;
    slot->v4l2_timestamp_ns = (int64_t)b->timestamp.tv_sec * 1000000000ll + (int64_t)b->timestamp.tv_usec * 1000ll;
    slot->dequeue_monotonic_ns = mono;
    slot->dequeue_raw_ns = raw;
    slot->width = node->width; slot->height = node->height; slot->stride = stride; slot->bytes = (uint32_t)bytes64;
    slot->copied_monotonic_ns = now_ns(CLOCK_MONOTONIC);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    __atomic_store_n(&slot->seq, slot->seq + 1, __ATOMIC_RELEASE);  // even: complete
    __atomic_store_n(&node->write_seq, number + 1, __ATOMIC_RELEASE);
    pthread_mutex_unlock(&lock);
}


static int subdev_number(int fd) {
    if (fd_subdev[fd] != -2) return fd_subdev[fd];
    char link[64], path[64] = {0};
    snprintf(link, sizeof link, "/proc/self/fd/%d", fd);
    int n = -1;
    if (readlink(link, path, sizeof path - 1) > 0 && !strncmp(path, "/dev/v4l-subdev", 15)) n = atoi(path + 15);
    fd_subdev[fd] = (signed char)(n > 127 ? -1 : n);
    return fd_subdev[fd];
}
static void event(int64_t raw, unsigned long request, uint32_t id, int sub, int64_t value) {
    uint64_t i = __atomic_fetch_add(&shm->event_seq, 0, __ATOMIC_ACQUIRE);
    // Single XRService control thread is expected; a racing writer can only overwrite one ring entry.
    __typeof__(shm->events[0]) *e = &shm->events[i % XRTAP_EVENTS];
    e->raw_ns = raw; e->request = request; e->control_id = id; e->subdev = sub; e->value = value;
    __atomic_store_n(&shm->event_seq, i + 1, __ATOMIC_RELEASE);
}

int ioctl(int fd, unsigned long request, ...) {
    va_list ap;
    va_start(ap, request);
    void *arg = va_arg(ap, void *);
    va_end(ap);
    if (!real_ioctl) real_ioctl = (int (*)(int, unsigned long, ...))dlsym(RTLD_NEXT, "ioctl");
    int result = real_ioctl(fd, request, arg);
    if (is_xrservice && result == 0 && request == VIDIOC_DQBUF) poll_enable();
    if (active && result == 0 && request == VIDIOC_DQBUF && arg) {
        const int saved = errno;
        const struct v4l2_buffer *b = (const struct v4l2_buffer *)arg;
        if (b->type == V4L2_BUF_TYPE_VIDEO_CAPTURE || b->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
            tap(fd, b, now_ns(CLOCK_MONOTONIC), now_ns(CLOCK_MONOTONIC_RAW));
        errno = saved;
    } else if (XRTAP_SUBDEV_EVENTS && active && result == 0 && arg && fd >= 0 && fd < MAX_FD && subdev_number(fd) >= 0 &&
               request != VIDIOC_DQEVENT) {  // diagnostic only, compiled out by default
        const int saved = errno;
        const int64_t raw = now_ns(CLOCK_MONOTONIC_RAW);
        const int sub = subdev_number(fd);
        if (request == VIDIOC_S_CTRL) {
            const struct v4l2_control *c = (const struct v4l2_control *)arg;
            event(raw, request, c->id, sub, c->value);
        } else if (request == VIDIOC_S_EXT_CTRLS) {
            const struct v4l2_ext_controls *c = (const struct v4l2_ext_controls *)arg;
            for (uint32_t i = 0; c->controls && i < c->count && i < 16; ++i)
                event(raw, request, c->controls[i].id, sub, c->controls[i].size ? 0 : c->controls[i].value64);
        } else if (request != VIDIOC_G_CTRL && request != VIDIOC_G_EXT_CTRLS && request != VIDIOC_QUERYCTRL) {
            // Unknown/private request: keep up to 12 payload bytes (word0 in control_id, words1..2 in value).
            uint32_t w[3] = {0, 0, 0};
            const unsigned size = _IOC_SIZE(request);
            if ((_IOC_DIR(request) & _IOC_WRITE) && size) memcpy(w, arg, size < 12 ? size : 12);
            event(raw, request, w[0], sub, (int64_t)((uint64_t)w[1] | ((uint64_t)w[2] << 32)));
        }
        errno = saved;
    } else if (active && result == 0 && (request == VIDIOC_S_FMT) && arg && fd >= 0 && fd < MAX_FD) {
        if (pthread_mutex_lock(&lock) == 0) {
            int index = node_for_fd(fd);
            if (active && index >= 0) shm->nodes[index].width = 0;  // re-read format on next frame
            pthread_mutex_unlock(&lock);
        }
    }
    return result;
}

int close(int fd) {
    static int (*real_close)(int);
    if (!real_close) real_close = (int (*)(int))dlsym(RTLD_NEXT, "close");
    if (active && fd >= 0 && fd < MAX_FD && pthread_mutex_lock(&lock) == 0) {
        fd_node[fd] = -2;  // fd numbers are reused
        fd_subdev[fd] = -2;
        for (int i = 0; i < MAX_MAPS; ++i) {
            if (maps[i].addr && maps[i].fd == fd) { munmap(maps[i].addr, maps[i].len); maps[i].addr = NULL; }
            if (mmaps[i].addr && mmaps[i].fd == fd) mmaps[i].addr = NULL;  // XRService unmaps its own MMAP buffers
        }
        pthread_mutex_unlock(&lock);
    }
    return real_close(fd);
}

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {
    if (!real_mmap) real_mmap = (void *(*)(void *, size_t, int, int, int, off_t))dlsym(RTLD_NEXT, "mmap");
    void *p = real_mmap(addr, len, prot, flags, fd, offset);
    if (active && p != MAP_FAILED && fd >= 0 && pthread_mutex_lock(&lock) == 0) {
        if (active && node_for_fd(fd) >= 0)
            for (int i = 0; i < MAX_MAPS; ++i)
                if (!mmaps[i].addr) { mmaps[i].fd = fd; mmaps[i].offset = offset; mmaps[i].addr = p; mmaps[i].len = len; break; }
        pthread_mutex_unlock(&lock);
    }
    return p;
}
void *mmap64(void *addr, size_t len, int prot, int flags, int fd, off_t offset) { return mmap(addr, len, prot, flags, fd, offset); }
