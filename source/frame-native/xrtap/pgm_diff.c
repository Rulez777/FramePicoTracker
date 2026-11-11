// pgm_diff a.pgm b.pgm: means and mean absolute difference of two same-size 8-bit PGMs for
// b as-is, flipped horizontally, vertically and rotated 180 degrees (diagnostic).
#include <stdio.h>
#include <stdlib.h>
static unsigned char *load(const char *path, int *w, int *h) {
    FILE *f = fopen(path, "rb");
    int max;
    if (!f || fscanf(f, "P5 %d %d %d", w, h, &max) != 3) { fprintf(stderr, "bad %s\n", path); exit(2); }
    fgetc(f);
    unsigned char *p = malloc((size_t)*w * *h);
    if (fread(p, 1, (size_t)*w * *h, f) != (size_t)*w * *h) { fprintf(stderr, "short %s\n", path); exit(2); }
    fclose(f);
    return p;
}
int main(int argc, char **argv) {
    if (argc != 3) return 2;
    int w, h, w2, h2;
    unsigned char *a = load(argv[1], &w, &h), *b = load(argv[2], &w2, &h2);
    if (w != w2 || h != h2) { printf("size %dx%d vs %dx%d\n", w, h, w2, h2); return 1; }
    double ma = 0, mb = 0, d[4] = {0, 0, 0, 0};
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            int v = a[(long)y * w + x];
            ma += v; mb += b[(long)y * w + x];
            d[0] += abs(v - b[(long)y * w + x]);
            d[1] += abs(v - b[(long)y * w + (w - 1 - x)]);
            d[2] += abs(v - b[(long)(h - 1 - y) * w + x]);
            d[3] += abs(v - b[(long)(h - 1 - y) * w + (w - 1 - x)]);
        }
    double n = (double)w * h;
    printf("meanA=%.2f meanB=%.2f mad=%.2f hflip=%.2f vflip=%.2f rot180=%.2f\n", ma / n, mb / n, d[0] / n, d[1] / n, d[2] / n, d[3] / n);
    return 0;
}
