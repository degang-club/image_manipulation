#ifndef LIBAFBEELDING_IMG_H
#define LIBAFBEELDING_IMG_H
#include <stdint.h>

#define AFB_MAGIC "AFB:D" // 0x05 Bytes

typedef enum {
    AFB_E_SUCCESS,
    AFB_E_FILE_ERROR,
    AFB_E_WRONG_TYPE,
} AFB_ERROR;

typedef enum {
    NONE,
    PALETTED,
    GRAYSCALE,
    TRUECOLOR,
} IMAGE_TYPE;

typedef struct {
    IMAGE_TYPE image_type;
    uint32_t width;
    uint32_t height;
    uint8_t *image_data;
    
    uint64_t palette_size;
    uint8_t *palette_data;
} IMAGE;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} COLOR;

typedef struct {
    uint64_t size;
    COLOR *colors;
} PALETTE;

IMAGE afb_image_init(void);
void afb_image_free(IMAGE *img);
AFB_ERROR afb_palette_save(IMAGE *img, char *path);
AFB_ERROR afb_image_save(IMAGE *img, char *path);
AFB_ERROR image_to_pal(IMAGE *img);
AFB_ERROR image_to_rgb(IMAGE *img);


#endif
