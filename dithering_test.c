#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "dither.h"
#include "format-tga.h"
#include "color_quantization.h"

int main(void)
{
	AFB_IMAGE src_img = afb_image_init();
	if (afb_format_tga_load
	    (&src_img, "test_images/TGA/kodim23/kodim23_rgb_no-rle_top-left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
		return 1;
	}
	AFB_IMAGE dst_img = afb_image_init();
	
	AFB_PALETTE pal = quantize_median_cut(src_img, 256);
	afb_dither_floyd_steinberg(&dst_img, &src_img, &pal);

	afb_image_save(&dst_img, "test_output_dither.afb");
    return 0;
}
