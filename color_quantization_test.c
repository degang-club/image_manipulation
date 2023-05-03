#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "tga.h"
#include "color_quantization.h"

int main(void)
{
	IMAGE img = afb_image_init();
	PALETTE pal;
	tga_load_file(&img, "../tga/test_images/kodim23_rgb_no-rle_top-left.tga");

	pal = quantize_median_cut(img, 256);
	for (int i=0; i < pal.size; i++) {
		printf("R: 0x%02x G: 0x%02x B: 0x%02x\n", pal.colors[i].red,
			   pal.colors[i].green, pal.colors[i].blue);
	}
	// afb_image_save(&img, "test_output.afb");
	// afb_palette_save(&img, "test_output_palette.afb");

    return 0;
}
