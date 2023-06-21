#ifndef LIBAFBEELDING_SCALE_H
#define LIBAFBEELDING_SCALE_H
#include <stdint.h>
#include "img.h"

AFB_ERROR afb_scale_nearest_neighbor(AFB_IMAGE *img, uint32_t w, uint32_t h);

#endif
