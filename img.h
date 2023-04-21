#ifndef LIBRGBA_IMG_H
#define LIBRGBA_IMG_H
#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t *imagedata;
} TEXTURE;

#endif