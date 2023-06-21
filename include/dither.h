#ifndef LIBAFBEELDING_DITHER_H
#define LIBAFBEELDING_DITHER_H
#include "img.h"

AFB_ERROR afb_dither_floyd_steinberg(AFB_IMAGE *dst, AFB_IMAGE *src,
                                     AFB_PALETTE *pal);
AFB_ERROR afb_dither_atkinson(AFB_IMAGE *dst, AFB_IMAGE *src, AFB_PALETTE *pal);
#endif
