#include <stdio.h>
#include "img.h"
#include "tga.h"

int main()
{
	IMAGE img = afb_image_init();

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

	tga_load_file(&img, "/home/robin/Projects/tga/test_images/kodim23_rgb_no-rle_top-left.tga");
	//tga_load_file(&img, "/home/robin/Projects/tga/test_images/kodim23_pal_no-rle_top_left.tga");
	//image_to_rgb(&img);
	image_to_pal(&img);
	afb_image_save(&img, "test_output.afb");
	afb_palette_save(&img.palette, "test_output_palette.afb");
	
	return 0;
}
