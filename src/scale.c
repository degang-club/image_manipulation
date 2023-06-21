#include <stdlib.h>
#include "scale.h"

static AFB_ERROR nearest_neighbor_rgb(IMAGE *img, uint32_t w, uint32_t h)
{
	unsigned int x_ratio = ((img->width << 16) / w);
	unsigned int y_ratio = ((img->height << 16) / h);
	unsigned int x2, y2;
	uint8_t *new_image_data = malloc((w * h) * 3);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			x2 = ((x * x_ratio) >> 16);
			y2 = ((y * y_ratio) >> 16);
			new_image_data[((y * w) + x) * 3] =
			    img->image_data[((y2 * img->width) + x2) * 3];
			new_image_data[((y * w) + x) * 3 + 1] =
			    img->image_data[((y2 * img->width) + x2) * 3 + 1];
			new_image_data[((y * w) + x) * 3 + 2] =
			    img->image_data[((y2 * img->width) + x2) * 3 + 2];
		}
	}

	free(img->image_data);
	img->image_data = new_image_data;
	img->width = w;
	img->height = h;

	return AFB_E_SUCCESS;
}

static AFB_ERROR nearest_neighbor_pal(IMAGE *img, uint32_t w, uint32_t h)
{
	unsigned int x_ratio = ((img->width << 16) / w);
	unsigned int y_ratio = ((img->height << 16) / h);
	unsigned int x2, y2;
	uint8_t *new_image_data = malloc(w * h);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			x2 = ((x * x_ratio) >> 16);
			y2 = ((y * y_ratio) >> 16);
			new_image_data[(y * w) + x] =
			    img->image_data[(y2 * img->width) + x2];
		}
	}

	free(img->image_data);
	img->image_data = new_image_data;
	img->width = w;
	img->height = h;

	return AFB_E_SUCCESS;
}

AFB_ERROR afb_scale_nearest_neighbor(IMAGE *img, uint32_t w, uint32_t h)
{
	switch (img->image_type) {
	case TRUECOLOR:
		return nearest_neighbor_rgb(img, w, h);
	case PALETTED:
		return nearest_neighbor_pal(img, w, h);
	default:
		return AFB_E_WRONG_TYPE;
	}
}
