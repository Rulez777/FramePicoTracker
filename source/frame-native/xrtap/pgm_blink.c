// pgm_blink on.pgm off.pgm: pixels bright (>200) in "on" and dark (<100) in "off" - a blinking source.
// Prints their count, bounding box and up to 12 cluster centres (diagnostic).
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
    if (w != w2 || h != h2) return 1;
    int n = 0, x0 = w, y0 = h, x1 = -1, y1 = -1;
    int cx[12], cy[12], cn[12], k = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const long i = (long)y * w + x;
            if (a[i] <= 200 || b[i] >= 100) continue;
            ++n;
            if (x < x0) x0 = x; if (y < y0) y0 = y; if (x > x1) x1 = x; if (y > y1) y1 = y;
            int j = 0;
            for (; j < k; ++j) if (abs(cx[j] / cn[j] - x) < 12 && abs(cy[j] / cn[j] - y) < 12) break;
            if (j == k && k < 12) { cx[k] = 0; cy[k] = 0; cn[k] = 0; ++k; }
            if (j < k) { cx[j] += x; cy[j] += y; ++cn[j]; }
        }
    printf("blinking_px=%d bbox=(%d,%d)-(%d,%d) clusters=%d:", n, x0, y0, x1, y1, k);
    for (int j = 0; j < k; ++j) printf(" (%d,%d)x%d", cx[j] / cn[j], cy[j] / cn[j], cn[j]);
    printf("\n");
    return 0;
}
