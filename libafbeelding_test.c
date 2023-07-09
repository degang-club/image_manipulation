#include <stdio.h>
#include "img.h"
#include "format-tga.h"

int main()
{
	AFB_IMAGE img_pal_top_left = afb_image_init();
	AFB_IMAGE img_pal_bottom_left = afb_image_init();
	AFB_IMAGE img_rgb_top_left = afb_image_init();
	AFB_IMAGE img_rgb_bottom_left = afb_image_init();

//  59K kodim23_gray_no-rle_bottom-left.tga
//  59K kodim23_gray_no-rle_top-left.tga
//  62K kodim23_gray_rle_bottom-left.tga
//  62K kodim23_gray_rle_top-left.tga
//  60K kodim23_pal_no-rle_bottom_left.tga
//  60K kodim23_pal_no-rle_top_left.tga
//  44K kodim23_pal_rle_bottom_left.tga
//  44K kodim23_pal_rle_top_left.tga
// 176K kodim23_rgb_no-rle_bottom_left.tga
//  37K kodim23_rgb_no-rle_top-left_128_96.tga
// 176K kodim23_rgb_no-rle_top-left.tga
// 175K kodim23_rgb_rle_bottom-left.tga
// 175K kodim23_rgb_rle_top-left.tga

	puts("load pal top left");
	if (afb_format_tga_load(&img_pal_top_left, "test_images/TGA/kodim23/kodim23_pal_no-rle_top_left.tga") != AFB_E_SUCCESS)
		puts("Error");

	puts("load pal bottom left");
	if (afb_format_tga_load(&img_pal_bottom_left, "test_images/TGA/kodim23/kodim23_pal_no-rle_bottom_left.tga") != AFB_E_SUCCESS)
		puts("Error");

	puts("load rgb top left");
	if (afb_format_tga_load(&img_rgb_top_left, "test_images/TGA/kodim23/kodim23_rgb_no-rle_top-left.tga") != AFB_E_SUCCESS)
		puts("Error");

	puts("load rgb bottom left");
	if (afb_format_tga_load(&img_rgb_bottom_left, "test_images/TGA/kodim23/kodim23_rgb_no-rle_bottom_left.tga") != AFB_E_SUCCESS)
		puts("Error");

	puts("save pal top left");
	afb_image_save(&img_pal_top_left, "test_output_pal_top_left.afb");

	puts("save pal bottom left");
	afb_image_save(&img_pal_bottom_left, "test_output_pal_bottom_left.afb");

	puts("save rgb top left");
	afb_image_save(&img_rgb_top_left, "test_output_rgb_top_left.afb");

	puts("save rgb bottom left");
	afb_image_save(&img_rgb_bottom_left, "test_output_rgb_bottom_left.afb");

	return 0;
}
