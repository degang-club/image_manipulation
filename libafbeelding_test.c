#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img.h"
#include "format-tga.h"

char* strcomb(unsigned int num, ...)
{
	va_list valist;

	char *combined_str;
	unsigned int combined_str_s = 1;

	char *tmp_str;
	unsigned int i;

	va_start(valist, num);
	combined_str = calloc(1, sizeof(char));

	for (i = 0; i < num; i++) {
		tmp_str = va_arg(valist, char*);

		combined_str_s += strlen(tmp_str);
		combined_str = realloc(combined_str, combined_str_s);

		strcat(combined_str, tmp_str);
	}
	va_end(valist);

	return combined_str;
}

char *substr(char *start, char *end) {
	int length = end - start;
	char *sub = calloc(length + 1, sizeof(char));
	strncpy(sub, start, length);
	sub[length] = '\0';
	return sub;
}

int main()
{
	const char tga_extension[] = ".tga";
	AFB_IMAGE img = afb_image_init();
	char *file_path;
	char *file_name;
	char *extension_start;

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

	char *dir_path = "./test_images/TGA/kodim23/";
	char *new_file_name;

	if (dir_path[strlen(dir_path) - 1] != '/')
		dir_path = strcat(dir_path, "/");

	DIR *dir = opendir(dir_path);
	struct dirent *entry;
	if(dir == NULL) {
		printf("Could not open provided directory!");
		return 1;
	}

	while((entry = readdir(dir)) != NULL) {
		extension_start = strrchr(entry->d_name, '.'); // Find last '.' to get extension 
		if(extension_start == NULL) continue;

		// Load tga into wad when it is a tga file
		if(strcmp(extension_start, tga_extension) == 0) {
			img = afb_image_init();

			file_path = strcomb(2, dir_path, entry->d_name);
			file_name = substr(entry->d_name, extension_start);
			printf("%s\n", file_name);

			if (afb_format_tga_load(&img, file_path) != AFB_E_SUCCESS) {
				puts("Error");
				continue;
			}

			new_file_name = calloc(strlen(file_name) + strlen(".afb") + 1, 1);
			strcat(new_file_name, file_name);
			strcat(new_file_name, ".afb");

			afb_image_save(&img, new_file_name);

			free(file_path);
			free(file_name);
			free(new_file_name);
			afb_image_free(&img);
		}
	}

	return 0;
}
