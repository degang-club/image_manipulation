#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include "img.h"

IMAGE afb_image_init(void)
{
	IMAGE img = { .image_type = NONE, .width = 0, .height = 0, .image_data = NULL,
		.palette_size = 0, .palette_data = NULL };
	return img;
}

void afb_image_free(IMAGE *img)
{
	if (img->image_data != NULL)
		free(img->image_data);
	
	if (img->palette_data != NULL)
		free(img->palette_data);
}

AFB_ERROR image_to_rgb(IMAGE *img)
{
	if(img->image_type == TRUECOLOR) return AFB_E_WRONG_TYPE;
	
	unsigned int image_size = img->width * img->height;
	
	if (img->image_type == PALETTED) {
		uint8_t *new_image_data = malloc(image_size * 3);
		for (int i=0; i < image_size; i++) {
			new_image_data[i * 3 + 0] = img->palette_data[img->image_data[i] * 3 + 0];
			new_image_data[i * 3 + 1] = img->palette_data[img->image_data[i] * 3 + 1];
			new_image_data[i * 3 + 2] = img->palette_data[img->image_data[i] * 3 + 2];
		}
		
		// Check if these are null
		free(img->image_data);
		free(img->palette_data);
		
		img->image_type = TRUECOLOR;
		img->image_data = new_image_data;
		img->palette_size = 0;
	}
	
	return AFB_E_SUCCESS;
}

AFB_ERROR image_to_pal(IMAGE *img)
{
	if(img->image_type == PALETTED) return AFB_E_WRONG_TYPE;

	unsigned int i_pal;
	unsigned int image_size = img->width * img->height;

	if (img->image_type == TRUECOLOR) {
		img->palette_size = 0;
		uint8_t *new_image_data = malloc(image_size);
		uint64_t alloced_palette = 96;
		uint8_t *new_palette = malloc(alloced_palette);
		uint8_t red, green, blue;

		for (unsigned int i=0; i < image_size; i++) {
			red = img->image_data[i * 3 + 0];			
			green = img->image_data[i * 3 + 1];			
			blue = img->image_data[i * 3 + 2];			

			// Check if color is present in the color pallete
			for (i_pal = 0; i_pal < img->palette_size; i_pal++) {
				if (red == new_palette[i_pal * 3 + 0] && green == new_palette[i_pal * 3 + 1] && blue == new_palette[i_pal * 3 + 2]) {
					new_image_data[i] = i_pal;
					break;
				}
			}
			
			// Add color to color pallete
			if (i_pal == img->palette_size) {

				if(img->palette_size * 3 >= alloced_palette) {
					alloced_palette += 96;
					new_palette = realloc(new_palette, alloced_palette);
				}

				new_palette[img->palette_size * 3 + 0] = red;
				new_palette[img->palette_size * 3 + 1] = green;
				new_palette[img->palette_size * 3 + 2] = blue;
				new_image_data[i] = img->palette_size;
				img->palette_size++;
			}
		}

		// Check if these are null
		free(img->image_data);
		free(img->palette_data);
		
		img->image_type = PALETTED;
		img->image_data = new_image_data;
		img->palette_data = new_palette;
	}
	return AFB_E_SUCCESS;
}

AFB_ERROR image_to_gray(IMAGE *img)
{
	if(img->image_type == GRAYSCALE) return AFB_E_WRONG_TYPE;
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_palette_save(IMAGE *img, char *path)
{
	if(img->image_type != PALETTED) return AFB_E_WRONG_TYPE;

	FILE *f = fopen(path, "w");
	
	if (f == NULL)
		return AFB_E_FILE_ERROR;

	fwrite(AFB_MAGIC, sizeof(AFB_MAGIC) - 1, 1, f);

	fwrite(&img->palette_size, 4, 1, f);
	uint32_t width_value = 1;
	fwrite(&width_value, 4, 1, f);
	
	fwrite(&img->palette_data[0], 3 * img->palette_size, 1, f);

	fclose(f);
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_image_save(IMAGE *img, char *path)
{
	int img_size;
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
		 	fwrite(&img->palette_data[img->image_data[i] * 3 + 0], 1, 1, f);
			fwrite(&img->palette_data[img->image_data[i] * 3 + 1], 1, 1, f);
			fwrite(&img->palette_data[img->image_data[i] * 3 + 2], 1, 1, f);
		}
	}

	fclose(f);
	return AFB_E_SUCCESS;
}
