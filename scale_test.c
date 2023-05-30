#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "img.h"
#include "tga.h"
#include "scale.h"

int main(void)
{
	IMAGE img = afb_image_init();
	if (tga_load_file(&img, "test_images/TGA/kodim23/kodim23_pal_no-rle_top_left.tga") != AFB_E_SUCCESS) {
		printf("Could not load image\n");
		return 1;
	}
	
    afb_scale_nearest_neighbor(&img, 600, 400);
	afb_image_save(&img, "test_output_scale_2x.afb");
    afb_scale_nearest_neighbor(&img, 150, 100);
	afb_image_save(&img, "test_output_scale_0.5x.afb");
    return 0;
}
