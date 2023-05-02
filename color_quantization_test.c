#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "color_quantization.h"

IMAGE read_imagedata()
{
	FILE *f = fopen("test_output.byt", "r");

    IMAGE img;

	fread(&img.width, sizeof(img.width), 1, f);
	fread(&img.height, sizeof(img.height), 1, f);

    img.image_data = malloc((img.width * img.height) * 3);

	for (size_t i = 0; i < img.width * img.height; i++)
	{
		fread(&img.image_data[i * 3], 1, 1, f);
		fread(&img.image_data[i * 3 + 1], 1, 1, f);
		fread(&img.image_data[i * 3 + 2], 1, 1, f);
	}

	fclose(f);
	return img;
}

int main(void)
{
	IMAGE img = read_imagedata();
	quantize_median_cut(img, 256);

	free(img.image_data);
    return 0;
}
