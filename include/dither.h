#ifndef LIBAFBEELDING_DITHER_H
#define LIBAFBEELDING_DITHER_H
#include "img.h"

AFB_ERROR afb_dither_floyd_steinberg(IMAGE *dst, IMAGE *src, PALETTE *pal);

#endif
