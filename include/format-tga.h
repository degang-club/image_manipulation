#ifndef LIBAFBEELDING_FORMAT_TGA_H
#define LIBAFBEELDING_FORMAT_TGA_H

#include "img.h"

AFB_ERROR afb_format_tga_load(AFB_IMAGE *img, char *path);
AFB_ERROR afb_format_tga_save(AFB_IMAGE *img, char *path);

#endif
