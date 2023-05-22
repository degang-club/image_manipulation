#include <stdio.h>
#include <stdlib.h>
#include "img.h"
#include "tga.h"

AFB_ERROR tga_load_file(IMAGE *img, char *path)
{
	FILE *f_tga;
	uint32_t image_size;
	uint8_t red, green, blue;
	int i;
	
	uint8_t idLength; // Unused
	uint8_t mapType;
	uint8_t imageType;
	
	uint16_t entryIndex; // Unused
	uint16_t entryLength;
	uint8_t bpp;
	
	uint16_t xOrigin; // Unused - Use these to flip image accordingly
	uint16_t yOrigin; // Unused
	uint16_t width;
	uint16_t height;
	uint8_t pixelSize; // Unused
	uint8_t descriptorByte; // Unused

	f_tga = fopen(path, "r");
	
	if (f_tga == NULL)
		return AFB_E_FILE_ERROR;
	
	fread(&idLength, sizeof(idLength), 1, f_tga);
	fread(&mapType, sizeof(mapType), 1, f_tga);
	fread(&imageType, sizeof(imageType), 1, f_tga);
	
	fread(&entryIndex, sizeof(entryIndex), 1, f_tga);
	fread(&entryLength, sizeof(entryLength), 1, f_tga);
	fread(&bpp, sizeof(bpp), 1, f_tga);
	
	fread(&xOrigin, sizeof(xOrigin), 1, f_tga);
	fread(&yOrigin, sizeof(yOrigin), 1, f_tga);
	fread(&width, sizeof(width), 1, f_tga);
	fread(&height, sizeof(height), 1, f_tga);
	fread(&pixelSize, sizeof(pixelSize), 1, f_tga);
	fread(&descriptorByte, sizeof(descriptorByte), 1, f_tga);

	img->width = width;
	img->height = height;

	if (mapType == TGA_MAP_COLORMAP) {
		img->palette.colors = malloc(entryLength * sizeof(*img->palette.colors));
		img->palette.size = entryLength;
		for (i=0; i < entryLength; i++) {
			fread(&blue, 1, 1, f_tga); // Blue
			fread(&green, 1, 1, f_tga); // Green
			fread(&red, 1, 1, f_tga); // Red
			img->palette.colors[i] = afb_to_rgba(red, green, blue, 0);
		}
	}

	image_size = width * height;

	switch (imageType) {
		case TGA_IMG_TRUECOLOR:
			img->image_type = TRUECOLOR;
			image_size *= 3;
			break;
		case TGA_IMG_COLORMAPPED:
			img->image_type = PALETTED;
			break;
		case TGA_IMG_BLACKWHITE:
			img->image_type = PALETTED;
			break;
	}

	img->image_data = malloc(image_size);
	
	if (imageType == TGA_IMG_TRUECOLOR) {
		for (i=0; i < image_size; i += 3) {
			fread(&img->image_data[i + 2], sizeof(img->image_data[0]), 1, f_tga);
			fread(&img->image_data[i + 1], sizeof(img->image_data[0]), 1, f_tga);
			fread(&img->image_data[i + 0], sizeof(img->image_data[0]), 1, f_tga);
		}
	} else {
		for (i=0; i < image_size; i++) {
			fread(&img->image_data[i], 1, 1, f_tga);
		}
	}
	
	return AFB_E_SUCCESS;
}
