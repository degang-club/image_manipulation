#include <stdio.h>
#include <stdlib.h>
#include "img.h"
#include "format-tga.h"

#define TGA_MAP_NOCOLORMAP 0
#define TGA_MAP_COLORMAP 1

#define TGA_IMG_NOIMAGEDATA 0
#define TGA_IMG_COLORMAPPED 1
#define TGA_IMG_TRUECOLOR 2
#define TGA_IMG_BLACKWHITE 3
#define TGA_IMG_RUNLENGTH_COLORMAPPED 9
#define TGA_IMG_RUNLENGTH_TRUECOLOR 10
#define TGA_IMG_RUNLENGTH_BLACKWHITE 11

AFB_ERROR afb_format_tga_load(AFB_IMAGE *img, char *path)
{
	FILE *f_tga;
	uint8_t red, green, blue, alpha;
	uint8_t order_rl;			// Order Right to Left
	uint8_t order_tb;			// Order Top to Bottom
	unsigned int read_bytes = 0;

	uint8_t idLength;           // Unused
	uint8_t mapType;
	uint8_t imageType;

	uint16_t entryIndex;        // Unused
	uint16_t entryLength;
	uint8_t bpp;

	uint16_t xOrigin;           // Unused - Used for aligning multiple images
	uint16_t yOrigin;           // on old graphics cards. Legacy feature.
	uint16_t width;
	uint16_t height;
	uint8_t pixelSize;          // Amount of bits per pixel
	uint8_t descriptorByte;     // xxxx0000 - Alpha depth
								// xxx0xxxx - Order right to left
								// xx0xxxxx - Order Top to bottom

	f_tga = fopen(path, "r");

	/* Return error if we can't open the file */
	if (f_tga == NULL)
		return AFB_E_FILE_ERROR;

	/* Reading the header */
	read_bytes += fread(&idLength, sizeof(idLength), 1, f_tga);
	read_bytes += fread(&mapType, sizeof(mapType), 1, f_tga);
	read_bytes += fread(&imageType, sizeof(imageType), 1, f_tga);

	read_bytes += fread(&entryIndex, sizeof(entryIndex), 1, f_tga);
	read_bytes += fread(&entryLength, sizeof(entryLength), 1, f_tga);
	read_bytes += fread(&bpp, sizeof(bpp), 1, f_tga);

	read_bytes += fread(&xOrigin, sizeof(xOrigin), 1, f_tga);
	read_bytes += fread(&yOrigin, sizeof(yOrigin), 1, f_tga);
	read_bytes += fread(&width, sizeof(width), 1, f_tga);
	read_bytes += fread(&height, sizeof(height), 1, f_tga);
	read_bytes += fread(&pixelSize, sizeof(pixelSize), 1, f_tga);
	read_bytes += fread(&descriptorByte, sizeof(descriptorByte), 1, f_tga);

	/* Check if we read the entire header (it's 12 because we read 12 items) */
	if (read_bytes != 12)
		return AFB_E_INVALID_FILE;

	/* Get the order settings from the descriptor byte */
	order_rl = (descriptorByte & 0x10) >> 4;
	order_tb = (descriptorByte & 0x20) >> 5;

	/* Set the image type */
	switch (imageType) {
	case TGA_IMG_TRUECOLOR:
	case TGA_IMG_RUNLENGTH_TRUECOLOR: // Fallthrough
		if (pixelSize == 24)
			img->image_type = TRUECOLOR;
		else if (pixelSize == 32)
			img->image_type = TRUECOLOR_ALPHA;
		break;
	case TGA_IMG_COLORMAPPED:
	case TGA_IMG_RUNLENGTH_COLORMAPPED: // Fallthrough
		img->image_type = PALETTED;
		break;
	case TGA_IMG_BLACKWHITE:
	case TGA_IMG_RUNLENGTH_BLACKWHITE: // Fallthrough
		img->image_type = GRAYSCALE;
		break;
	}

	/* Set the width & height */
	img->width = width;
	img->height = height;

	/* Read the palette if it's present */
	if (mapType == TGA_MAP_COLORMAP) {
		img->palette.colors =
		    malloc(entryLength * sizeof(*img->palette.colors));
		img->palette.size = entryLength;
		for (unsigned int i = 0; i < entryLength; i++) {
			fread(&blue, 1, 1, f_tga);  // Blue
			fread(&green, 1, 1, f_tga); // Green
			fread(&red, 1, 1, f_tga);   // Red
			img->palette.colors[i] = afb_to_rgba(red, green, blue, 0xff);
		}
	}

	/* Allocate memory for the image data */
	if (img->image_type == TRUECOLOR) {
		img->image_data = malloc(width * height * 3);
	} else if (img->image_type == TRUECOLOR_ALPHA) {
		img->image_data = malloc(width * height * 4);
	} else {
		img->image_data = malloc(width * height);
	}

	// TODO Read RLE compressed image
	/* Loop through image and copy image data */
	/* We check if the image is top-to-bottom and/or right-to-left ordered.
	 * We go forwards or backwards through the loop to accommodate for all 
	 * the types of TGA images. */
	if (imageType == TGA_IMG_TRUECOLOR) {
		if (pixelSize == 24) {
			for (int y = (order_tb ? 0 : img->height -1);
					(order_tb ? y < img->height : y != -1); order_tb ? y++ : y--) {
				for (int x = (order_rl ? img->width : 0);
						(order_rl ? x != 0 : x < img->width); order_rl ? x-- : x++) {
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 3 + 2],
						sizeof(img->image_data[0]),	1,  f_tga);
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 3 + 1],
						sizeof(img->image_data[0]),	1, f_tga);
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 3 + 0],
						sizeof(img->image_data[0]),	1, f_tga);
				}
			}
		} else if (pixelSize == 32) {
			for (int y = (order_tb ? 0 : img->height -1);
					(order_tb ? y < img->height : y != -1); order_tb ? y++ : y--) {
				for (int x = (order_rl ? img->width : 0);
						(order_rl ? x != 0 : x < img->width); order_rl ? x-- : x++) {
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 4 + 2],
						sizeof(img->image_data[0]), 1, f_tga);
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 4 + 1],
						sizeof(img->image_data[0]),	1, f_tga);
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 4 + 0],
						sizeof(img->image_data[0]),	1, f_tga);
					fread(&img->image_data[afb_xy_to_1d(x, y, img->width) * 4 + 3],
						sizeof(img->image_data[0]),	1, f_tga);
				}
			}
		}
	} else if (imageType == TGA_IMG_COLORMAPPED || imageType == TGA_IMG_BLACKWHITE) {
		for (int y = (order_tb ? 0 : img->height -1);
				(order_tb ? y < img->height : y != -1); order_tb ? y++ : y--) {
			for (int x = (order_rl ? img->width : 0);
					(order_rl ? x != 0 : x < img->width); order_rl ? x-- : x++) {
				fread(&img->image_data[afb_xy_to_1d(x, y, img->width)], 1, 1, f_tga);
			}
		}
	} else if (imageType == TGA_IMG_RUNLENGTH_TRUECOLOR) {
		unsigned int i = 0;
		uint8_t img_packet_header;
		uint64_t image_size = (width * height);

		if (pixelSize == 24)
			image_size *= 3;
		else if (pixelSize == 32)
			image_size *= 4;

		while (i < image_size) {
			/* Read packet header and pixel data */
			fread(&img_packet_header, sizeof(img_packet_header), 1, f_tga);

			if ((img_packet_header >> 7) & 0x1) { /* Check if RLE packet */
				fread(&blue, sizeof(blue), 1, f_tga);
				fread(&green, sizeof(green), 1, f_tga);
				fread(&red, sizeof(red), 1, f_tga);
				if (pixelSize == 32)
					fread(&alpha, sizeof(alpha), 1, f_tga);

				/* Repeat pixel */
				for (int x = 0; x != (img_packet_header & 0x7f) + 1; x++) {
					img->image_data[i++] = red;
					img->image_data[i++] = green;
					img->image_data[i++] = blue;
					if (pixelSize == 32)
						img->image_data[i++] = alpha;
				}
			} else { /* Raw packet */
				/* Read pixels */
				for (int x = 0; x != (img_packet_header & 0x7f) + 1; x++) {
					fread(&blue, sizeof(blue), 1, f_tga);
					fread(&green, sizeof(green), 1, f_tga);
					fread(&red, sizeof(red), 1, f_tga);
					if (pixelSize == 32)
						fread(&alpha, sizeof(alpha), 1, f_tga);

					img->image_data[i++] = red;
					img->image_data[i++] = green;
					img->image_data[i++] = blue;
					if (pixelSize == 32)
						img->image_data[i++] = alpha;
				}
			}
		}
		
		/* Flip image horizontally */
		if (!order_tb) {
			for (unsigned int y = 0; y < img->height / 2; y++) {
				for (unsigned int x = 0; x < img->width; x++) {
					red = img->image_data[afb_xy_to_1d(x, y, width) * 3];
					green = img->image_data[afb_xy_to_1d(x, y, width) * 3 + 1];
					blue = img->image_data[afb_xy_to_1d(x, y, width) * 3 + 2];

					img->image_data[afb_xy_to_1d(x, y, width) * 3] = img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3];
					img->image_data[afb_xy_to_1d(x, y, width) * 3 + 1] = img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3 + 1];
					img->image_data[afb_xy_to_1d(x, y, width) * 3 + 2] = img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3 + 2];

					img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3] = red;
					img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3 + 1] = green;
					img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width) * 3 + 2] = blue;
				}
			}
		}
	} else if (imageType == TGA_IMG_RUNLENGTH_COLORMAPPED || imageType == TGA_IMG_RUNLENGTH_BLACKWHITE) {
		unsigned int i = 0;
		uint8_t img_packet_header;
		uint8_t pixel;
		while (i < (width * height)) {
			/* Read packet header and pixel data */
			fread(&img_packet_header, sizeof(img_packet_header), 1, f_tga);

			if ((img_packet_header >> 7) & 0x1) { /* Check if RLE packet */
				fread(&pixel, sizeof(pixel), 1, f_tga);
				/* Repeat pixel */
				for (int x = 0; x != (img_packet_header & 0x7f) + 1; x++) {
					img->image_data[i] = pixel;
					i++;
				}
			} else { /* Raw packet */
				/* Read pixels */
				for (int x = 0; x != (img_packet_header & 0x7f) + 1; x++) {
					fread(&pixel, sizeof(pixel), 1, f_tga);
					img->image_data[i] = pixel;
					i++;
				}
			}
		}

		/* Flip image horizontally */
		if (!order_tb) {
			for (unsigned int y = 0; y < img->height / 2; y++) {
				for (unsigned int x = 0; x < img->width; x++) {
					pixel = img->image_data[afb_xy_to_1d(x, y, width)];
					img->image_data[afb_xy_to_1d(x, y, width)] = img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width)];
					img->image_data[
						afb_xy_to_1d(x, (height - y - 1), width)] = pixel;
				}
			}
		}
	}

	fclose(f_tga);

	return AFB_E_SUCCESS;
}
