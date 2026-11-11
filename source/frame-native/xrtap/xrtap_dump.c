// xrtap_dump <seconds>: print every tapped frame as "device v4l2_ts_seconds mean" (mean of every 16th pixel).
#define _GNU_SOURCE
#include "xrtap.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

static int64_t now_ns(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (int64_t)t.tv_sec * 1000000000ll + t.tv_nsec; }

int main(int argc, char **argv) {
    double seconds = argc > 1 ? atof(argv[1]) : 5;
    int fd = open("/dev/shm/xrtap", O_RDWR | O_CLOEXEC);
    if (fd < 0) { perror("open"); return 2; }
    xrtap_shm *shm = mmap(NULL, sizeof(xrtap_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED || shm->magic != XRTAP_MAGIC || shm->version != XRTAP_VERSION) { fprintf(stderr, "bad shm\n"); return 2; }
    uint64_t seen[XRTAP_NODES];
    for (uint32_t i = 0; i < XRTAP_NODES; ++i) seen[i] = shm->nodes[i].write_seq;
    const int64_t end = now_ns() + (int64_t)(seconds * 1e9);
    while (now_ns() < end) {
        __atomic_store_n(&shm->reader_heartbeat_ns, now_ns(), __ATOMIC_RELEASE);
        for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i) {
            xrtap_node *node = &shm->nodes[i];
            uint64_t ws = __atomic_load_n(&node->write_seq, __ATOMIC_ACQUIRE);
            if (ws - seen[i] > XRTAP_SLOTS) seen[i] = ws - XRTAP_SLOTS;
            for (; seen[i] < ws; ++seen[i]) {
                xrtap_slot *s = &node->slots[seen[i] % XRTAP_SLOTS];
                uint64_t a = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                uint64_t sum = 0, count = 0;
                for (uint32_t y = 0; y < s->height; y += 16)
                    for (uint32_t x = 0; x < s->width; x += 16) { sum += s->pixels[(size_t)y * s->stride + x]; ++count; }
                int64_t ts = s->v4l2_timestamp_ns;
                uint64_t b = __atomic_load_n(&s->seq, __ATOMIC_ACQUIRE);
                if (!(a & 1) && a == b) printf("%s %.9f %.2f\n", node->device, ts / 1e9, count ? (double)sum / count : 0);
            }
        }
        usleep(500);
    }
    return 0;
}
