// xrtap_events [count]: print the last subdev control events recorded by the tap,
// interleaved with the latest frame timestamps of each node (all CLOCK_MONOTONIC_RAW).
#include "xrtap.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int count = argc > 1 ? atoi(argv[1]) : 64;
    int fd = open("/dev/shm/xrtap", O_RDONLY | O_CLOEXEC);
    if (fd < 0) { perror("open /dev/shm/xrtap"); return 2; }
    const xrtap_shm *shm = mmap(NULL, sizeof(xrtap_shm), PROT_READ, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED || shm->magic != XRTAP_MAGIC || shm->version != XRTAP_VERSION) { fprintf(stderr, "bad shm\n"); return 2; }
    uint64_t n = shm->event_seq;
    printf("events=%llu\n", (unsigned long long)n);
    uint64_t from = n > (uint64_t)count ? n - count : 0;
    if (n - from > XRTAP_EVENTS) from = n - XRTAP_EVENTS;
    for (uint64_t i = from; i < n; ++i) {
        const __typeof__(shm->events[0]) *e = &shm->events[i % XRTAP_EVENTS];
        printf("%.6f subdev%d req=0x%08llx id=0x%08x value=%lld w1=%u w2=%u\n", e->raw_ns / 1e9, e->subdev,
               (unsigned long long)e->request, e->control_id, (long long)e->value, (unsigned)(e->value & 0xffffffff), (unsigned)((uint64_t)e->value >> 32));
    }
    for (uint32_t i = 0; i < shm->nodes_used && i < XRTAP_NODES; ++i) {
        const xrtap_node *node = &shm->nodes[i];
        printf("%s write_seq=%llu last ts:", node->device, (unsigned long long)node->write_seq);
        for (uint64_t k = node->write_seq > 6 ? node->write_seq - 6 : 0; k < node->write_seq; ++k)
            printf(" %.6f", node->slots[k % XRTAP_SLOTS].v4l2_timestamp_ns / 1e9);
        printf("\n");
    }
    return 0;
}
