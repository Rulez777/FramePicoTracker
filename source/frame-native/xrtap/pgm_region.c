// pgm_region file.pgm x0 y0 x1 y1: max and count of pixels >150 inside a rectangle (diagnostic).
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    if (argc != 6) return 2;
    FILE *f = fopen(argv[1], "rb");
    int w, h, max;
    if (!f || fscanf(f, "P5 %d %d %d", &w, &h, &max) != 3) return 2;
    fgetc(f);
    unsigned char *p = malloc((size_t)w * h);
    if (fread(p, 1, (size_t)w * h, f) != (size_t)w * h) return 2;
    int x0 = atoi(argv[2]), y0 = atoi(argv[3]), x1 = atoi(argv[4]), y1 = atoi(argv[5]), m = 0, n = 0;
    for (int y = y0; y < y1 && y < h; ++y)
        for (int x = x0; x < x1 && x < w; ++x) {
            const int v = p[(long)y * w + x];
            if (v > m) m = v;
            if (v > 150) ++n;
        }
    printf("%d %d\n", m, n);
    return 0;
}
