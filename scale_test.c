#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "format-tga.h"
#include "scale.h"

int main(void)
{
	AFB_IMAGE img_rgb = afb_image_init();
	AFB_IMAGE img_pal = afb_image_init();

	if (afb_format_tga_load
	    (&img_rgb, "test_images/TGA/kodim23/kodim23_rgb_no-rle_top-left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
		return 1;
	}
	
	if (afb_format_tga_load
	    (&img_pal, "test_images/TGA/kodim23/kodim23_pal_no-rle_top_left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
		return 1;
	}
	
    afb_scale_nearest_neighbor(&img_rgb, 600, 400);
	afb_image_save(&img_rgb, "test_output_scale_rgb_2x.afb");
    afb_scale_nearest_neighbor(&img_rgb, 150, 100);
	afb_image_save(&img_rgb, "test_output_scale_rgb_0.5x.afb");

    afb_scale_nearest_neighbor(&img_pal, 600, 400);
	afb_image_save(&img_pal, "test_output_scale_pal_2x.afb");
    afb_scale_nearest_neighbor(&img_pal, 150, 100);
	afb_image_save(&img_pal, "test_output_scale_pal_0.5x.afb");
    return 0;
}
