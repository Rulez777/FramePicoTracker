// xrtap_region_log <seconds> <device> x0 y0 x1 y1: for every tapped frame of <device> print
// "<dequeue_monotonic_ns> <v4l2_ts_ns> <gap_ms> <max> <count_gt150>" inside the rectangle (diagnostic,
// used to find which exposures contain the tracker LED flashes during a phase sweep).
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

int main(int argc, char **argv) {
    if (argc != 7) { fprintf(stderr, "Usage: xrtap_region_log <seconds> <device> x0 y0 x1 y1\n"); return 2; }
    const int64_t end = now_ns() + (int64_t)(atof(argv[1]) * 1e9);
    const int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
    xrtap_shm *shm = NULL;
    while (!shm && now_ns() < end) {  // the ring appears once the tap is enabled
        int fd = open("/dev/shm/xrtap", O_RDWR | O_CLOEXEC);
        if (fd >= 0) {
            void *p = mmap(NULL, sizeof(xrtap_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            if (p != MAP_FAILED && ((xrtap_shm *)p)->magic == XRTAP_MAGIC) shm = p;
            else if (p != MAP_FAILED) munmap(p, sizeof(xrtap_shm));
        }
        if (!shm) usleep(100000);
    }
    if (!shm) return 1;
    uint64_t seen = 0;
    int node = -1;
    int64_t last_ts = 0;
    while (now_ns() < end) {
        __atomic_store_n(&shm->reader_heartbeat_ns, now_ns(), __ATOMIC_RELEASE);
        if (node < 0)
            for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i)
                if (!strcmp(shm->nodes[i].device, argv[2])) { node = (int)i; seen = shm->nodes[i].write_seq; }
        if (node >= 0) {
            xrtap_node *n = &shm->nodes[node];
            const uint64_t ws = __atomic_load_n(&n->write_seq, __ATOMIC_ACQUIRE);
            if (ws < seen || ws - seen > XRTAP_SLOTS) seen = ws > XRTAP_SLOTS ? ws - XRTAP_SLOTS : 0;
            for (; seen < ws; ++seen) {
                xrtap_slot *s = &n->slots[seen % XRTAP_SLOTS];
                const uint64_t a = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                const int64_t ts = s->v4l2_timestamp_ns, dq = s->dequeue_monotonic_ns;
                int m = 0, c = 0;
                for (int y = y0; y < y1 && y < (int)s->height; ++y)
                    for (int x = x0; x < x1 && x < (int)s->width; ++x) {
                        const int v = s->pixels[(size_t)y * s->stride + x];
                        if (v > m) m = v;
                        if (v > 150) ++c;
                    }
                if ((a & 1) || a != __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE)) continue;
                printf("%lld %lld %.2f %d %d\n", (long long)dq, (long long)ts, last_ts ? (ts - last_ts) / 1e6 : 0.0, m, c);
                last_ts = ts;
            }
        }
        usleep(1000);
    }
    return 0;
}
