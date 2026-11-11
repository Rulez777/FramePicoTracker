// Prints the offsets of BOOTTIME and MONOTONIC_RAW against MONOTONIC (diagnostic).
#include <stdio.h>
#include <time.h>
static double ms(struct timespec a, struct timespec b) { return ((b.tv_sec - a.tv_sec) * 1e9 + (b.tv_nsec - a.tv_nsec)) / 1e6; }
int main(void) {
    struct timespec m, b, r;
    clock_gettime(CLOCK_MONOTONIC, &m);
    clock_gettime(CLOCK_BOOTTIME, &b);
    clock_gettime(CLOCK_MONOTONIC_RAW, &r);
    printf("boottime-monotonic=%.3f ms raw-monotonic=%.3f ms\n", ms(m, b), ms(m, r));
    return 0;
}
