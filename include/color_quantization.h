#ifndef LIBAFBEELDING_COLOR_QUANTIZATION_H
#define LIBAFBEELDING_COLOR_QUANTIZATION_H
#include "img.h"

typedef struct {
	PALETTE palette;
	uint8_t ranges[3];
	uint8_t largest_channel;
} BUCKET;

void sort_colors_channel(BUCKET *bucket);
PALETTE quantize_median_cut(IMAGE img, unsigned int palette_size);

PALETTE afb_unique_colors(uint8_t *img_data, unsigned int img_size);

#endif
