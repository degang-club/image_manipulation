#ifndef LIBAFBEELDING_COLOR_QUANTIZATION_H
#define LIBAFBEELDING_COLOR_QUANTIZATION_H
#include "img.h"

typedef struct {
    unsigned int size;
    uint8_t *begin;
    uint8_t ranges[3];
    uint8_t largest_channel;
    uint8_t red_avg;
    uint8_t green_avg;
    uint8_t blue_avg;
} BUCKET;

int get_colormapped_pixel(int r, int g, int b, BUCKET **colormap, int size);
void sort_colors_channel(BUCKET *bucket);
void quantize_median_cut(IMAGE img, unsigned int palette_size);

#endif
