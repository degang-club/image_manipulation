#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "color_quantization.h"

TEXTURE read_imagedata()
{
	FILE *f = fopen("test_output.byt", "r");

    TEXTURE tex;

	fread(&tex.width, sizeof(tex.width), 1, f);
	fread(&tex.height, sizeof(tex.height), 1, f);

    tex.imagedata = malloc((tex.width * tex.height) * 3);

	for (size_t i = 0; i < tex.width * tex.height; i++)
	{
		fread(&tex.imagedata[i * 3], 1, 1, f);
		fread(&tex.imagedata[i * 3 + 1], 1, 1, f);
		fread(&tex.imagedata[i * 3 + 2], 1, 1, f);
	}

	fclose(f);
	return tex;
}

int main(void)
{
	TEXTURE tex = read_imagedata();
	quantize_median_cut(tex, 256);

	free(tex.imagedata);
    return 0;
}