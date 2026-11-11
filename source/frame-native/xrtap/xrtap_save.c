// xrtap_save <seconds> <out_dir> [device...]: save every tapped frame of the given devices
// (default /dev/video0 /dev/video3) as <out_dir>/<videoN>_<v4l2_ts_ns>.pgm (diagnostic, disk-heavy).
#define _GNU_SOURCE
#include "xrtap.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static int64_t now_ns(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (int64_t)t.tv_sec * 1000000000ll + t.tv_nsec; }
static uint8_t buffer[XRTAP_MAX_BYTES];

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "Usage: xrtap_save <seconds> <out_dir> [device...]\n"); return 2; }
    const char *defaults[] = {"/dev/video0", "/dev/video3"};
    const char **wanted = argc > 3 ? (const char **)argv + 3 : defaults;
    int nwanted = argc > 3 ? argc - 3 : 2;
    int fd = open("/dev/shm/xrtap", O_RDWR | O_CLOEXEC);
    if (fd < 0) { perror("open"); return 2; }
    xrtap_shm *shm = mmap(NULL, sizeof(xrtap_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED || shm->magic != XRTAP_MAGIC || shm->version != XRTAP_VERSION) { fprintf(stderr, "bad shm\n"); return 2; }
    uint64_t seen[XRTAP_NODES];
    for (uint32_t i = 0; i < XRTAP_NODES; ++i) seen[i] = shm->nodes[i].write_seq;
    const int64_t end = now_ns() + (int64_t)(atof(argv[1]) * 1e9);
    while (now_ns() < end) {
        __atomic_store_n(&shm->reader_heartbeat_ns, now_ns(), __ATOMIC_RELEASE);
        for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i) {
            xrtap_node *node = &shm->nodes[i];
            int keep = 0;
            for (int k = 0; k < nwanted; ++k) keep |= !strcmp(node->device, wanted[k]);
            uint64_t ws = __atomic_load_n(&node->write_seq, __ATOMIC_ACQUIRE);
            if (!keep) { seen[i] = ws; continue; }
            if (ws - seen[i] > XRTAP_SLOTS) seen[i] = ws - XRTAP_SLOTS;
            for (; seen[i] < ws; ++seen[i]) {
                xrtap_slot *s = &node->slots[seen[i] % XRTAP_SLOTS];
                uint64_t a = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                uint32_t w = s->width, h = s->height, stride = s->stride;
                int64_t ts = s->v4l2_timestamp_ns;
                if (w > stride || (size_t)stride * h > sizeof buffer) continue;
                memcpy(buffer, s->pixels, (size_t)stride * h);
                uint64_t b = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                if ((a & 1) || a != b) continue;
                char path[512];
                snprintf(path, sizeof path, "%s/%s_%lld.pgm", argv[2], strrchr(node->device, '/') + 1, (long long)ts);
                FILE *f = fopen(path, "wb");
                if (!f) continue;
                fprintf(f, "P5\n%u %u\n255\n", w, h);
                for (uint32_t y = 0; y < h; ++y) fwrite(buffer + (size_t)y * stride, 1, w, f);
                fclose(f);
            }
        }
        usleep(500);
    }
    return 0;
}
