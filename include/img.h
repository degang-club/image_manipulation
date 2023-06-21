#ifndef LIBAFBEELDING_IMG_H
#define LIBAFBEELDING_IMG_H
#include <stdint.h>

#define AFB_MAGIC "AFB:D"       // 0x05 Bytes

#define afb_rgba_get_r(c) ((c >> 24) & 0xff)
#define afb_rgba_get_g(c) ((c >> 16) & 0xff)
#define afb_rgba_get_b(c) ((c >> 8) & 0xff)
#define afb_rgba_get_a(c) (c & 0xff)
#define afb_to_rgba(r,g,b,a) ((r << 24) + (g << 16) + (b << 8) + a)
#define afb_xy_to_1d(x, y, width) ((y * width) + x)

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
	uint64_t size;
	uint32_t *colors;
} AFB_PALETTE;

typedef struct {
	IMAGE_TYPE image_type;
	uint32_t width;
	uint32_t height;
	uint8_t *image_data;

	AFB_PALETTE palette;
} AFB_IMAGE;

AFB_IMAGE afb_image_init(void);
void afb_image_free(AFB_IMAGE *img);

AFB_ERROR afb_palette_save(AFB_PALETTE *pal, char *path);
AFB_ERROR afb_image_save(AFB_IMAGE *img, char *path);

AFB_ERROR image_to_pal(AFB_IMAGE *img, AFB_PALETTE * pal);
AFB_ERROR image_to_rgb(AFB_IMAGE *img);

AFB_PALETTE afb_palette_init(void);
void afb_palette_free(AFB_PALETTE *pal);

unsigned int afb_closest_color(int red, int green, int blue, AFB_PALETTE *pal);
AFB_IMAGE afb_copy_image(AFB_IMAGE *img);

#endif
