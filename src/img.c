#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img.h"

IMAGE afb_image_init(void)
{
	return (IMAGE){ .image_type = NONE, .width = 0, .height = 0, .image_data = NULL,
		.palette.size = 0, .palette.colors = NULL };
}

void afb_image_free(IMAGE *img)
{
	if (img->image_data != NULL)
		free(img->image_data);
	
	if (img->palette.colors != NULL)
		free(img->palette.colors);
}

AFB_ERROR image_to_rgb(IMAGE *img)
{
	if(img->image_type == TRUECOLOR) return AFB_E_WRONG_TYPE;
	
	unsigned int image_size = img->width * img->height;
	
	if (img->image_type == PALETTED) {
		uint8_t *new_image_data = malloc(image_size * 3);
		for (int i=0; i < image_size; i++) {
			new_image_data[i * 3 + 0] = afb_rgba_get_r(img->palette.colors[img->image_data[i]]);
			new_image_data[i * 3 + 1] = afb_rgba_get_g(img->palette.colors[img->image_data[i]]);
			new_image_data[i * 3 + 2] = afb_rgba_get_b(img->palette.colors[img->image_data[i]]);
		}
		
		// Check if these are null
		free(img->image_data);
		free(img->palette.colors);
		
		img->image_type = TRUECOLOR;
		img->image_data = new_image_data;
		img->palette.size = 0;
	}
	
	return AFB_E_SUCCESS;
}

AFB_ERROR image_to_pal(IMAGE *img, PALETTE *pal)
{
	if(img->image_type == PALETTED) return AFB_E_WRONG_TYPE;

	unsigned int image_size = img->width * img->height;

	if (img->image_type == TRUECOLOR) {
		uint8_t *new_image_data = malloc(image_size);
		int red, green, blue;
		double current_squared_distance = 0;
		double previous_squared_distance = DBL_MAX;
		int smallest_distance_index = 0;

		for (unsigned int i=0; i < image_size; i++) {
			current_squared_distance = 0;
			previous_squared_distance = DBL_MAX;
			smallest_distance_index = 0;

			red = img->image_data[i * 3 + 0];
			green = img->image_data[i * 3 + 1];
			blue = img->image_data[i * 3 + 2];
			
			for (int y = 0; y < pal->size; y++) {
				current_squared_distance = pow(red - (uint8_t)afb_rgba_get_r(pal->colors[y]), 2)
					+ pow(green - (uint8_t)afb_rgba_get_g(pal->colors[y]), 2)
					+ pow(blue - (uint8_t)afb_rgba_get_b(pal->colors[y]), 2);
				if (current_squared_distance < previous_squared_distance)
				{
					previous_squared_distance = current_squared_distance;
					smallest_distance_index = y;
				}
			}
			new_image_data[i] = smallest_distance_index;
		}

		// Check if these are null
		if (img->image_data != NULL)
			free(img->image_data);

		if (img->palette.colors != NULL)
			free(img->palette.colors);
		
		img->image_type = PALETTED;
		img->image_data = new_image_data;
		img->palette.size = pal->size;

		img->palette.colors = malloc(pal->size * sizeof(uint32_t));
		memcpy(img->palette.colors, pal->colors, pal->size * sizeof(uint32_t));
	}
	return AFB_E_SUCCESS;
}

AFB_ERROR image_to_gray(IMAGE *img)
{
	if(img->image_type == GRAYSCALE) return AFB_E_WRONG_TYPE;
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_palette_save(PALETTE *pal, char *path)
{
	FILE *f = fopen(path, "w");
	uint8_t r,g,b;
	
	if (f == NULL)
		return AFB_E_FILE_ERROR;

	fwrite(AFB_MAGIC, sizeof(AFB_MAGIC) - 1, 1, f);

	fwrite(&pal->size, 4, 1, f);
	uint32_t width_value = 1;
	fwrite(&width_value, 4, 1, f);
	
	for (int i=0; i < pal->size; i++) {
		r = afb_rgba_get_r(pal->colors[i]);
		g = afb_rgba_get_g(pal->colors[i]);
		b = afb_rgba_get_b(pal->colors[i]);
		fwrite(&r, 1, 1, f);
		fwrite(&g, 1, 1, f);
		fwrite(&b, 1, 1, f);
	}

	fclose(f);
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_image_save(IMAGE *img, char *path)
{
	int img_size;
	uint8_t red, green, blue;
	FILE *f = fopen(path, "w");
	
	if (f == NULL)
		return AFB_E_FILE_ERROR;

	fwrite(AFB_MAGIC, sizeof(AFB_MAGIC) - 1, 1, f);

	fwrite(&img->width, 4, 1, f);
	fwrite(&img->height, 4, 1, f);
	
	img_size = img->width * img->height;

	if(img->image_type == TRUECOLOR) {
		img_size *= 3;
		for (int i = 0; i < img_size; i++) {
			fwrite(&img->image_data[i], 1, 1, f);
		}
	} else if (img->image_type == PALETTED) {
		for (int i = 0; i < img_size; i++) {
			red = afb_rgba_get_r(img->palette.colors[img->image_data[i]]);
			green = afb_rgba_get_g(img->palette.colors[img->image_data[i]]);
			blue = afb_rgba_get_b(img->palette.colors[img->image_data[i]]);
		 	fwrite(&red, 1, 1, f);
			fwrite(&green, 1, 1, f);
			fwrite(&blue, 1, 1, f);
		}
	}

	fclose(f);
	return AFB_E_SUCCESS;
}

PALETTE afb_palette_init(void)
{
	return (PALETTE){ .size = 0, .colors = NULL };
}

void afb_palette_free(PALETTE *pal)
{
	free(pal->colors);
}
