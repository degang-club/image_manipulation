#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "tga.h"
#include "color_quantization.h"

int main(void)
{
	IMAGE img = afb_image_init();
	if (tga_load_file(&img, "../tga/test_images/kodim23_rgb_no-rle_top-left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
	}
	
	PALETTE pal = quantize_median_cut(img, 256);
	afb_palette_save(&pal, "test_output_palette.afb");
    return 0;
}
