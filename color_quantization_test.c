#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "format-tga.h"
#include "color_quantization.h"

int main(void)
{
	AFB_IMAGE img = afb_image_init();
	if (afb_format_tga_load
	    (&img, "test_images/TGA/kodim23/kodim23_rgb_no-rle_top-left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
		return 1;
	}
	
	AFB_PALETTE pal = afb_quantize_median_cut(img, 256);
	image_to_pal(&img, &pal);
	afb_image_save(&img, "test_output.afb");
	afb_palette_save(&pal, "test_output_palette.afb");
    return 0;
}
