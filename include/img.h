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
    uint64_t size;
    uint32_t *colors;
} PALETTE;

#define afb_rgba_get_r(c) ((c >> 24) & 0xff)
#define afb_rgba_get_g(c) ((c >> 16) & 0xff)
#define afb_rgba_get_b(c) ((c >> 8) & 0xff)
#define afb_rgba_get_a(c) (c & 0xff)
#define afb_to_rgba(r,g,b,a) ((r << 24) + (g << 16) + (b << 8) + a)

IMAGE afb_image_init(void);
void afb_image_free(IMAGE *img);
AFB_ERROR afb_palette_save(PALETTE *pal, char *path);
AFB_ERROR afb_image_save(IMAGE *img, char *path);
AFB_ERROR image_to_pal(IMAGE *img);
AFB_ERROR image_to_rgb(IMAGE *img);
PALETTE afb_palette_init(void);
void afb_palette_free(PALETTE *pal);

// COLOR get_col(uint8_t *data, int i);

#endif
