#include <stdlib.h>
#include "scale.h"

AFB_ERROR afb_scale_nearest_neighbor(IMAGE *img, uint32_t w, uint32_t h)
{
	if (img->image_type == TRUECOLOR) return AFB_E_WRONG_TYPE;
	if (img->image_type == PALETTED) {
        unsigned int x_ratio = ((img->width << 16) / w);
		unsigned int y_ratio = ((img->height << 16) / h);
        unsigned int x2,y2;
		unsigned int new_image_size = w * h;
		uint8_t *new_image_data = malloc(new_image_size);
		for (int y=0; y < h; y++) {
			for (int x=0; x < w; x++) {
                x2 = ((x * x_ratio) >> 16);
                y2 = ((y * y_ratio) >> 16);
				new_image_data[(y * w) + x] = img->image_data[(y2 * img->width) + x2];
			}
		}
		free(img->image_data);
		img->image_data = new_image_data;
		img->width = w;
		img->height = h;
	}
	return AFB_E_SUCCESS;
}
