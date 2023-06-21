#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "img.h"
#include "dither.h"

#define RED 0
#define GREEN 1
#define BLUE 2

AFB_ERROR afb_dither_floyd_steinberg(AFB_IMAGE *dst, AFB_IMAGE *src, 
                                     AFB_PALETTE *pal)
{
	if (src->image_type != TRUECOLOR)
		return AFB_E_WRONG_TYPE;
	unsigned int img_size = src->width * src->height;
	uint8_t *src_img_data = malloc(img_size * 3);

	dst->width = src->width;
	dst->height = src->height;
	dst->image_type = PALETTED;
	dst->image_data = malloc(img_size);

	dst->palette.size = pal->size;
	dst->palette.colors = malloc(sizeof(uint32_t) * pal->size);
	memcpy(dst->palette.colors, pal->colors, sizeof(uint32_t) * pal->size);

	uint8_t old_px_r = 0;
	uint8_t old_px_g = 0;
	uint8_t old_px_b = 0;

	uint8_t new_px_r = 0;
	uint8_t new_px_g = 0;
	uint8_t new_px_b = 0;

	int quant_err_r = 0;
	int quant_err_g = 0;
	int quant_err_b = 0;

	memcpy(src_img_data, src->image_data, img_size * 3);

	for (int i = 0; i < img_size; i++) {

		// oldpixel := pixels[x][y]
		old_px_r = src_img_data[i * 3];
		old_px_g = src_img_data[i * 3 + 1];
		old_px_b = src_img_data[i * 3 + 2];

		// newpixel := find_closest_palette_color(oldpixel)
		int new_px_index = afb_closest_color(old_px_r, old_px_g, old_px_b, pal);
		new_px_r = (uint8_t) afb_rgba_get_r(pal->colors[new_px_index]);
		new_px_g = (uint8_t) afb_rgba_get_g(pal->colors[new_px_index]);
		new_px_b = (uint8_t) afb_rgba_get_b(pal->colors[new_px_index]);

		// pixels[x][y] := newpixel
		dst->image_data[i] = new_px_index;

		// quant_error := oldpixel - newpixel
		quant_err_r = old_px_r - new_px_r;
		quant_err_g = old_px_g - new_px_g;
		quant_err_b = old_px_b - new_px_b;

		// pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
		// pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
		// pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
		// pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16

		// red
		if (i % src->width != src->width - 1) { // check if we're at the end of the image
			src_img_data[(i + 1) * 3] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + 1) * 3] + ((quant_err_r * 7) / 16), 255),
							 0);
		}
		if (i + src->width < img_size) {
			if (i % src->width != 0) {
				src_img_data[(i - 1 + src->width) * 3] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i - 1 + src->width) * 3] +
				         ((quant_err_r * 3) / 16), 255), 0);
			}
			src_img_data[(i + src->width) * 3] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + src->width) * 3] +
			         ((quant_err_r * 5) / 16), 255), 0);
			if (i % src->width != src->width - 1) {
				src_img_data[(i + 1 + src->width) * 3] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i + 1 + src->width) * 3] +
				         ((quant_err_r * 1) / 16), 255), 0);
			}
		}
		// green
		if (i % src->width != src->width - 1) {
			src_img_data[(i + 1) * 3 + 1] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + 1) * 3 + 1] + ((quant_err_g * 7) / 16),
			         255), 0);
		}
		if (i + src->width < img_size) {
			if (i % src->width != 0) {
				src_img_data[(i - 1 + src->width) * 3 + 1] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i - 1 + src->width) * 3 + 1] +
				         ((quant_err_g * 3) / 16), 255), 0);
			}
			src_img_data[(i + src->width) * 3 + 1] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + src->width) * 3 + 1] +
			         ((quant_err_g * 5) / 16), 255), 0);
			if (i % src->width != src->width - 1) {
				src_img_data[(i + 1 + src->width) * 3 + 1] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i + 1 + src->width) * 3 + 1] +
				         ((quant_err_g * 1) / 16), 255), 0);
			}
		}
		// blue
		if (i % src->width != src->width - 1) {
			src_img_data[(i + 1) * 3 + 2] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + 1) * 3 + 2] + ((quant_err_b * 7) / 16),
			         255), 0);
		}
		if (i + src->width < img_size) {
			if (i % src->width != 0) {
				src_img_data[(i - 1 + src->width) * 3 + 2] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i - 1 + src->width) * 3 + 2] +
				         ((quant_err_b * 3) / 16), 255), 0);
			}
			src_img_data[(i + src->width) * 3 + 2] =
			    (uint8_t) MAX(MIN(
					src_img_data[(i + src->width) * 3 + 2] +
			         ((quant_err_b * 5) / 16), 255), 0);
			if (i % src->width != src->width - 1) {
				src_img_data[(i + 1 + src->width) * 3 + 2] =
				    (uint8_t) MAX(MIN(
						src_img_data[(i + 1 + src->width) * 3 + 2] +
				         ((quant_err_b * 1) / 16), 255), 0);
			}
		}
	}

	free(src_img_data);

	return AFB_E_SUCCESS;
}

AFB_ERROR afb_dither_atkinson(AFB_IMAGE *dst, AFB_IMAGE *src, AFB_PALETTE *pal)
{
	/* For every pixel, find the closest match in the palette and push part of
	 * the error to the surrounding pixels like so (the pound sign represents
	 * the current pixel):
	 *
	 *         [ ...        #   1/8  1/8  ... ]
	 *         [ ...  1/8  1/8  1/8       ... ]
	 *         [ ...       1/8            ... ]
	 *
	 * In this case, 'error' refers to the difference between the match and the
	 * actual color.
	 */

	if (src->image_type != TRUECOLOR)
		return AFB_E_WRONG_TYPE;
	unsigned int img_size = src->width * src->height;
	uint8_t *src_img_data = malloc(img_size * 3);

	dst->width = src->width;
	dst->height = src->height;
	dst->image_type = PALETTED;
	dst->image_data = malloc(img_size);

	dst->palette.size = pal->size;
	dst->palette.colors = malloc(sizeof(uint32_t) * pal->size);
	memcpy(dst->palette.colors, pal->colors, sizeof(uint32_t) * pal->size);

	uint8_t old_px_r = 0;
	uint8_t old_px_g = 0;
	uint8_t old_px_b = 0;

	uint8_t new_px_r = 0;
	uint8_t new_px_g = 0;
	uint8_t new_px_b = 0;

	int quant_err_r = 0;
	int quant_err_g = 0;
	int quant_err_b = 0;

	memcpy(src_img_data, src->image_data, img_size * 3);

	for (int i = 0; i < img_size; i++) {
		// oldpixel := pixels[x][y]
		old_px_r = src_img_data[i * 3 + RED];
		old_px_g = src_img_data[i * 3 + GREEN];
		old_px_b = src_img_data[i * 3 + BLUE];

		// newpixel := find_closest_palette_color(oldpixel)
		int new_px_index = afb_closest_color(old_px_r, old_px_g, old_px_b, pal);
		new_px_r = (uint8_t) afb_rgba_get_r(pal->colors[new_px_index]);
		new_px_g = (uint8_t) afb_rgba_get_g(pal->colors[new_px_index]);
		new_px_b = (uint8_t) afb_rgba_get_b(pal->colors[new_px_index]);

		// pixels[x][y] := newpixel
		dst->image_data[i] = new_px_index;

		// quant_error := oldpixel - newpixel
		quant_err_r = (((old_px_r - new_px_r) * 1) / 8);
		quant_err_g = (((old_px_g - new_px_g) * 1) / 8);
		quant_err_b = (((old_px_b - new_px_b) * 1) / 8);

		// x: +1 y: 0
		if (i % src->width < src->width - 1) {
			src_img_data[(i + 1) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1) * 3 + RED] + quant_err_r, 255), 0);

			src_img_data[(i + 1) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1) * 3 + GREEN] + quant_err_g, 255), 0);

			src_img_data[(i + 1) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1) * 3 + BLUE] + quant_err_b, 255), 0);
		}

		// x: -1 y: +1
		if (i + src->width < img_size && i % src->width != 0) {
			src_img_data[(i - 1 + src->width) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i - 1 + src->width) * 3 + RED] + quant_err_r, 255), 0);
			src_img_data[(i - 1 + src->width) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i - 1 + src->width) * 3 + GREEN] + quant_err_g, 255), 0);
			src_img_data[(i - 1 + src->width) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i - 1 + src->width) * 3 + BLUE] + quant_err_b, 255), 0);
		}

		// x: +1 y: +1
		if (i + src->width < img_size && i % src->width < src->width - 1) {
			src_img_data[(i + 1 + src->width) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1 + src->width) * 3 + RED] + quant_err_r, 255), 0);
			src_img_data[(i + 1 + src->width) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1 + src->width) * 3 + GREEN] + quant_err_g, 255), 0);
			src_img_data[(i + 1 + src->width) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 1 + src->width) * 3 + BLUE] + quant_err_b, 255), 0);
		}

		// x: 0 y: +1
		if (i + src->width < img_size) {
			src_img_data[(i + src->width) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + src->width) * 3 + RED] + quant_err_r, 255), 0);
			src_img_data[(i + src->width) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + src->width) * 3 + GREEN] + quant_err_g, 255), 0);
			src_img_data[(i + src->width) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + src->width) * 3 + BLUE] + quant_err_b, 255), 0);
		}

		// x: +2 y: 0
		if (i % src->width < src->width - 2) {
			src_img_data[(i + 2) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 2) * 3 + RED] + quant_err_r, 255), 0);
			src_img_data[(i + 2) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 2) * 3 + GREEN] + quant_err_g, 255), 0);
			src_img_data[(i + 2) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + 2) * 3 + BLUE] + quant_err_b, 255), 0);
		}

		// x: 0 y: +2
		if (i + (src->width * 2) < img_size) {
			src_img_data[(i + (src->width * 2)) * 3 + RED] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + (src->width * 2)) * 3 + RED] + quant_err_r, 255), 0);
			src_img_data[(i + (src->width * 2)) * 3 + GREEN] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + (src->width * 2)) * 3 + GREEN] + quant_err_g, 255), 0);
			src_img_data[(i + (src->width * 2)) * 3 + BLUE] =
			(uint8_t) MAX(MIN(
				src_img_data[(i + (src->width * 2)) * 3 + BLUE] + quant_err_b, 255), 0);
		}
	}

	free(src_img_data);

	return AFB_E_SUCCESS;
}
