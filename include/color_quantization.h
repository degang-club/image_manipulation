#ifndef LIBAFBEELDING_COLOR_QUANTIZATION_H
#define LIBAFBEELDING_COLOR_QUANTIZATION_H
#include "img.h"

AFB_PALETTE afb_quantize_median_cut(AFB_IMAGE img, unsigned int palette_size);
AFB_PALETTE afb_unique_colors(uint8_t *img_data, unsigned int img_size);

#endif
