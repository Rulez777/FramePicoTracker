// xrtap_probe <seconds> [pgm_dir]: attach to /dev/shm/xrtap, keep the reader
// heartbeat alive, and report per-node frame rate and latency:
//   dq-ts   = DQBUF time minus the V4L2 buffer timestamp
//   read-ts = time this reader saw the frame minus the V4L2 buffer timestamp
// Optionally writes the last frame of each node as PGM.
#define _GNU_SOURCE
#include "xrtap.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static int64_t now_ns(clockid_t id) {
    struct timespec t;
    clock_gettime(id, &t);
    return (int64_t)t.tv_sec * 1000000000ll + t.tv_nsec;
}
static int cmp(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return (x > y) - (x < y); }
static void stats(const char *name, double *v, int n) {
    if (!n) { printf("  %s n=0\n", name); return; }
    qsort(v, n, sizeof *v, cmp);
    printf("  %s n=%d min=%.3f p50=%.3f p90=%.3f max=%.3f ms\n", name, n, v[0], v[n / 2], v[n * 9 / 10], v[n - 1]);
}

int main(int argc, char **argv) {
    double seconds = argc > 1 ? atof(argv[1]) : 10;
    const char *dir = argc > 2 ? argv[2] : NULL;
    int fd = open("/dev/shm/xrtap", O_RDWR | O_CLOEXEC);
    if (fd < 0) { perror("open /dev/shm/xrtap"); return 2; }
    xrtap_shm *shm = mmap(NULL, sizeof(xrtap_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) { perror("mmap"); return 2; }
    if (shm->magic != XRTAP_MAGIC || shm->version != XRTAP_VERSION) { fprintf(stderr, "bad magic/version\n"); return 2; }
    printf("writer_pid=%d nodes=%u\n", shm->writer_pid, shm->nodes_used);
    enum { CAP = 4096 };
    static double dq[XRTAP_NODES][CAP], rd[XRTAP_NODES][CAP], cp[XRTAP_NODES][CAP], period[XRTAP_NODES][CAP];
    int n[XRTAP_NODES] = {0}, np[XRTAP_NODES] = {0};
    uint64_t seen[XRTAP_NODES] = {0}, first[XRTAP_NODES] = {0}, skip0[XRTAP_NODES];
    int64_t last_ts[XRTAP_NODES] = {0};
    for (uint32_t i = 0; i < XRTAP_NODES; ++i) skip0[i] = shm->nodes[i].skipped;
    const int64_t start = now_ns(CLOCK_MONOTONIC), end = start + (int64_t)(seconds * 1e9);
    while (now_ns(CLOCK_MONOTONIC) < end) {
        __atomic_store_n(&shm->reader_heartbeat_ns, now_ns(CLOCK_MONOTONIC), __ATOMIC_RELEASE);
        for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i) {
            xrtap_node *node = &shm->nodes[i];
            uint64_t ws = __atomic_load_n(&node->write_seq, __ATOMIC_ACQUIRE);
            if (!first[i]) { first[i] = ws ? ws : 1; seen[i] = ws; continue; }
            while (seen[i] < ws) {
                xrtap_slot *s = &node->slots[seen[i] % XRTAP_SLOTS];
                uint64_t a = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                int64_t ts = s->v4l2_timestamp_ns, dqm = s->dequeue_monotonic_ns, cpm = s->copied_monotonic_ns;
                uint64_t number = s->frame_number;
                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                uint64_t b = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                int64_t now = now_ns(CLOCK_MONOTONIC);
                if (!(a & 1) && a == b && number == seen[i] && n[i] < CAP) {
                    dq[i][n[i]] = (dqm - ts) / 1e6; rd[i][n[i]] = (now - ts) / 1e6; cp[i][n[i]] = (cpm - dqm) / 1e6; n[i]++;
                    if (last_ts[i] && np[i] < CAP) period[i][np[i]++] = (ts - last_ts[i]) / 1e6;
                    last_ts[i] = ts;
                }
                seen[i]++;
            }
        }
        usleep(500);
    }
    for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i) {
        xrtap_node *node = &shm->nodes[i];
        double secs = seconds;
        printf("%s %ux%u stride=%u fmt=%.4s frames=%d (%.2f fps) skipped=%llu\n", node->device, node->width, node->height,
               node->stride, (const char *)&node->pixelformat, n[i], n[i] / secs,
               (unsigned long long)(node->skipped - skip0[i]));
        stats("dq-ts  ", dq[i], n[i]);
        stats("read-ts", rd[i], n[i]);
        stats("copy   ", cp[i], n[i]);
        stats("period ", period[i], np[i]);
        if (dir && node->write_seq) {
            xrtap_slot *s = &node->slots[(node->write_seq - 1) % XRTAP_SLOTS];
            char path[512];
            const char *base = strrchr(node->device, '/');
            snprintf(path, sizeof path, "%s/%s.pgm", dir, base ? base + 1 : node->device);
            FILE *f = fopen(path, "wb");
            if (f) {
                fprintf(f, "P5\n%u %u\n255\n", s->stride, s->height);
                fwrite(s->pixels, 1, (size_t)s->stride * s->height, f);
                fclose(f);
                printf("  wrote %s\n", path);
            }
        }
    }
    return 0;
}
