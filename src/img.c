#include <stdio.h>
#include <float.h>
#include <stdlib.h>
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

AFB_ERROR image_to_pal(IMAGE *img)
{
	if(img->image_type == PALETTED) return AFB_E_WRONG_TYPE;

	unsigned int i_pal;
	unsigned int image_size = img->width * img->height;

	if (img->image_type == TRUECOLOR) {
		img->palette.size = 0;
		uint8_t *new_image_data = malloc(image_size);
		uint64_t alloced_palette = 96;
		PALETTE new_palette;
		new_palette.colors = malloc(alloced_palette);
		uint8_t red, green, blue;

		for (unsigned int i=0; i < image_size; i++) {
			red = img->image_data[i * 3 + 0];
			green = img->image_data[i * 3 + 1];
			blue = img->image_data[i * 3 + 2];

			// Check if color is present in the color pallete
			for (i_pal = 0; i_pal < img->palette.size; i_pal++) {
				if (red == afb_rgba_get_r(new_palette.colors[i_pal])
					&& green == afb_rgba_get_g(new_palette.colors[i_pal])
					&& blue == afb_rgba_get_b(new_palette.colors[i_pal])) {
					new_image_data[i] = i_pal;
					break;
				}
			}
			
			// Add color to color pallete
			if (i_pal == img->palette.size) {

				if(img->palette.size * 4 >= alloced_palette) {
					alloced_palette += 96;
					new_palette.colors = realloc(new_palette.colors, alloced_palette * sizeof(*new_palette.colors));
				}

				new_palette.colors[img->palette.size] = afb_to_rgba(red, green, blue, 0);
				new_image_data[i] = img->palette.size;
				img->palette.size++;
			}
		}

		// Check if these are null
		free(img->image_data);
		free(img->palette.colors);
		
		img->image_type = PALETTED;
		img->image_data = new_image_data;
		img->palette.colors = new_palette.colors;
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
