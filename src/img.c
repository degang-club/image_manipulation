#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "img.h"

static int pow_of_two(int x)
{
	return x * x;
}

AFB_IMAGE afb_image_init(void)
{
	return (AFB_IMAGE) {
	.image_type = NONE,.width = 0,.height = 0,.image_data =
	        NULL,.palette.size = 0,.palette.colors = NULL};
}

void afb_image_free(AFB_IMAGE *img)
{
	if (img->image_data != NULL)
		free(img->image_data);

	if (img->palette.colors != NULL)
		free(img->palette.colors);
}

unsigned int afb_closest_color(int red, int green, int blue, AFB_PALETTE *pal)
{
	int current_squared_distance = 0;
	int previous_squared_distance = INT_MAX;
	int smallest_distance_index = 0;

	for (int y = 0; y < pal->size; y++) {
		current_squared_distance =
		    pow_of_two(red - (uint8_t) afb_rgba_get_r(pal->colors[y]))
		    + pow_of_two(green - (uint8_t) afb_rgba_get_g(pal->colors[y]))
		    + pow_of_two(blue - (uint8_t) afb_rgba_get_b(pal->colors[y]));
		if (current_squared_distance < previous_squared_distance) {
			previous_squared_distance = current_squared_distance;
			smallest_distance_index = y;
		}
	}
	return smallest_distance_index;
}

AFB_IMAGE afb_copy_image(AFB_IMAGE *img)
{
	AFB_IMAGE img_copy = afb_image_init();
	unsigned int img_data_size;
	unsigned int pal_colors_size;

	img_copy.image_type = img->image_type;
	img_copy.width = img->width;
	img_copy.height = img->height;

	if (img->image_type == TRUECOLOR)
		img_data_size = img->width * img->height * 3;
	else
		img_data_size = img->width * img->height;

	img_copy.image_data = malloc(img_data_size);
	memcpy(img_copy.image_data, img->image_data, img_data_size);

	if (img->image_type == PALETTED) {
		img_copy.palette.size = img->palette.size;
		pal_colors_size = img->palette.size * sizeof(uint32_t);

		img_copy.palette.colors = malloc(pal_colors_size);
		memcpy(img_copy.palette.colors, img->palette.colors, pal_colors_size);
	}

	return img_copy;
}

AFB_ERROR image_to_rgb(AFB_IMAGE *img)
{
	if (img->image_type == TRUECOLOR)
		return AFB_E_WRONG_TYPE;

	unsigned int image_size = img->width * img->height;

	if (img->image_type == PALETTED) {
		uint8_t *new_image_data = malloc(image_size * 3);
		for (int i = 0; i < image_size; i++) {
			new_image_data[i * 3 + 0] =
			    afb_rgba_get_r(img->palette.colors[img->image_data[i]]);
			new_image_data[i * 3 + 1] =
			    afb_rgba_get_g(img->palette.colors[img->image_data[i]]);
			new_image_data[i * 3 + 2] =
			    afb_rgba_get_b(img->palette.colors[img->image_data[i]]);
		}

		// Check if these are null
		free(img->image_data);
		free(img->palette.colors);

		img->image_type = TRUECOLOR;
		img->image_data = new_image_data;
		img->palette.size = 0;
	}

	return AFB_E_SUCCESS;
}

struct kd_node {
	uint64_t index;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	struct kd_node *left;
	struct kd_node *right;
};

struct color {
	unsigned int index;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

static int cmp_r(const void *a, const void *b)
{
	return ((*(struct color *) a).red >
	        (*(struct color *) b).red) - ((*(struct color *) a).red <
	                                      (*(struct color *) b).red);
}

static int cmp_g(const void *a, const void *b)
{
	return ((*(struct color *) a).green >
	        (*(struct color *) b).green) - ((*(struct color *) a).green <
	                                        (*(struct color *) b).green);
}

static int cmp_b(const void *a, const void *b)
{
	return ((*(struct color *) a).blue >
	        (*(struct color *) b).blue) - ((*(struct color *) a).blue <
	                                       (*(struct color *) b).blue);
}

struct kd_node * construct_kd_node_recursive(struct color *colors,
									uint64_t size, unsigned int depth)
{
	if(size <= 0) return NULL;

	struct kd_node *parent_node = malloc(sizeof(struct kd_node));

	unsigned int size_left = size / 2;
	unsigned int size_right = size - 1 - size_left;

	int (*sort_function)();
	switch(depth % 3) { // Depth determines compare axis
		case 0: sort_function = cmp_r; break;
		case 1: sort_function = cmp_g; break;
		case 2: sort_function = cmp_b; break;
	}

	qsort(colors, size, sizeof(struct color), sort_function);

	// Assign values of the median point
	parent_node->index = colors[size_left].index;
	parent_node->red = colors[size_left].red;
	parent_node->green = colors[size_left].green;
	parent_node->blue = colors[size_left].blue;

	// Recurse with both sides of the array
	parent_node->left = construct_kd_node_recursive(colors, size_left,
									depth + 1);
	parent_node->right = construct_kd_node_recursive(&colors[size_left + 1],
									size_right, depth + 1);

	return parent_node;
}

struct kd_node * construct_kd_tree_from_pal(AFB_PALETTE *pal)
{
	struct kd_node *root_node;
	struct color *colors = malloc(sizeof(struct color) * pal->size);

	// Create array with colors and their indices
	for (unsigned int i=0; i < pal->size; i++) {
		colors[i] = (struct color){
			.index = i,
			.red = (uint8_t)afb_rgba_get_r(pal->colors[i]),
			.green = (uint8_t)afb_rgba_get_g(pal->colors[i]),
			.blue = (uint8_t)afb_rgba_get_b(pal->colors[i]),
		};
	}

	root_node = construct_kd_node_recursive(colors, pal->size, 0);
	free(colors);

	return root_node;
}

void traverse_kd_tree(struct kd_node *root_node,
	struct kd_node **current_best,
	unsigned int *best_distance,
	uint8_t red, uint8_t green, uint8_t blue,
	unsigned int depth)
{
	bool side;
	switch(depth % 3) { // Depth determines split axis
		case 0: side = red > root_node->red; break;
		case 1: side = green > root_node->green; break;
		case 2: side = blue > root_node->blue; break;
	}

	bool is_leaf = root_node->right == NULL && root_node->left == NULL;

	struct kd_node *selected = side ? root_node->right : root_node->left;

	if(is_leaf) {
		// Calculate Taxicab distance
		unsigned int distance = abs(red - (uint8_t) root_node->red)
				    + abs(green - (uint8_t) root_node->green)
				    + abs(blue - (uint8_t) root_node->blue);

		if(distance < *best_distance) {
			*best_distance = distance;
			*current_best = root_node;
		}
		return;
	}

	// Try to traverse deeper if this is not a leaf node and the other side
	// is not occupied
	if(selected == NULL) {
		selected = side ? root_node->left : root_node->right;
	}

	traverse_kd_tree(selected, current_best, best_distance, red, green, blue,
					depth + 1);

	// Boundary check for current node to check for sneaky nodes in
	// other sections
	unsigned int boundary_distance;
	switch(depth % 3) {
		case 0: boundary_distance = abs(root_node->red - red); break;
		case 1: boundary_distance = abs(root_node->green - green); break;
		case 2: boundary_distance = abs(root_node->blue - blue); break;
	}

	if(boundary_distance < *best_distance) {
		// Check taxicab distance to the node itself when the boundary
		// is closer than best
		unsigned int distance = abs(red - (uint8_t) root_node->red)
				    + abs(green - (uint8_t) root_node->green)
				    + abs(blue - (uint8_t) root_node->blue);

		if(distance < *best_distance) {
			*best_distance = distance;
			*current_best = root_node;
		}

		// Traverse further into the other side of tree to check for
		// potential candidates
		struct kd_node *opposite_node = side ? root_node->left : root_node->right;
		if(opposite_node == NULL)
			return;

		traverse_kd_tree(opposite_node, current_best, best_distance,
				red, green, blue, depth + 1);
	}
}

struct kd_node * search_kd_tree(struct kd_node *root_node,
	uint8_t red, uint8_t green, uint8_t blue)
{
	struct kd_node **current_best = malloc(sizeof(struct kd_node*));
	*current_best = NULL;
	unsigned int best_distance = UINT_MAX;
	traverse_kd_tree(root_node, current_best, &best_distance,
			red, green, blue, 0);
	return *current_best;
}

void kd_tree_free(struct kd_node *root_node)
{
	if(root_node->left != NULL) kd_tree_free(root_node->left);
	if(root_node->right != NULL) kd_tree_free(root_node->right);

	free(root_node);
}

AFB_ERROR image_to_pal(AFB_IMAGE *img, AFB_PALETTE *pal)
{
	if (img->image_type == PALETTED)
		return AFB_E_WRONG_TYPE;

	unsigned int image_size = img->width * img->height;
	unsigned int errors = 0;
	unsigned int same_values = 0;

	if (img->image_type == TRUECOLOR) {
		struct kd_node *root_node = construct_kd_tree_from_pal(pal);

		uint8_t *new_image_data = malloc(image_size);
		uint8_t red, green, blue;

		for (unsigned int i = 0; i < image_size; i++) {
			red = img->image_data[i * 3 + 0];
			green = img->image_data[i * 3 + 1];
			blue = img->image_data[i * 3 + 2];

			struct kd_node *found_node = search_kd_tree(root_node,
											red, green, blue);

			new_image_data[i] = found_node->index;
		}

		// Check if these are null
		if (img->image_data != NULL)
			free(img->image_data);

		if (img->palette.colors != NULL)
			free(img->palette.colors);

		img->image_type = PALETTED;
		img->image_data = new_image_data;
		img->palette.size = pal->size;

		img->palette.colors = malloc(pal->size * sizeof(uint32_t));
		memcpy(img->palette.colors, pal->colors, pal->size * sizeof(uint32_t));
		kd_tree_free(root_node);
	} else if (img->image_type == GRAYSCALE) {
		img->palette.size = 256;
		img->image_type = PALETTED;

		img->palette.colors = malloc(img->palette.size * sizeof(uint32_t));
		for (unsigned int i=0; i < img->palette.size; i++) {
			img->palette.colors[i] = afb_to_rgba(i, i, i, 0xff);
		}
	}

	return AFB_E_SUCCESS;
}

AFB_ERROR image_to_gray(AFB_IMAGE *img)
{
	if (img->image_type == GRAYSCALE)
		return AFB_E_WRONG_TYPE;
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_palette_save(AFB_PALETTE *pal, char *path)
{
	FILE *f = fopen(path, "w");
	uint8_t r, g, b;

	if (f == NULL)
		return AFB_E_FILE_ERROR;

	fwrite(AFB_MAGIC, sizeof(AFB_MAGIC) - 1, 1, f);

	fwrite(&pal->size, 4, 1, f);
	uint32_t width_value = 1;
	fwrite(&width_value, 4, 1, f);

	for (int i = 0; i < pal->size; i++) {
		r = afb_rgba_get_r(pal->colors[i]);
		g = afb_rgba_get_g(pal->colors[i]);
		b = afb_rgba_get_b(pal->colors[i]);
		fwrite(&r, 1, 1, f);
		fwrite(&g, 1, 1, f);
		fwrite(&b, 1, 1, f);
	}

	fclose(f);
	return AFB_E_SUCCESS;
}

AFB_ERROR afb_image_save(AFB_IMAGE *img, char *path)
{
	int img_size;
	uint8_t r, g, b;
	FILE *f = fopen(path, "w");

	if (f == NULL)
		return AFB_E_FILE_ERROR;

	fwrite(AFB_MAGIC, sizeof(AFB_MAGIC) - 1, 1, f);

	fwrite(&img->width, 4, 1, f);
	fwrite(&img->height, 4, 1, f);

	img_size = img->width * img->height;

	if (img->image_type == TRUECOLOR) {
		img_size *= 3;
		for (int i = 0; i < img_size; i++) {
			fwrite(&img->image_data[i], 1, 1, f);
		}
	} else if (img->image_type == PALETTED) {
		for (int i = 0; i < img_size; i++) {
			r = afb_rgba_get_r(img->palette.colors[img->image_data[i]]);
			g = afb_rgba_get_g(img->palette.colors[img->image_data[i]]);
			b = afb_rgba_get_b(img->palette.colors[img->image_data[i]]);
			fwrite(&r, 1, 1, f);
			fwrite(&g, 1, 1, f);
			fwrite(&b, 1, 1, f);
		}
	}

	fclose(f);
	return AFB_E_SUCCESS;
}

AFB_PALETTE afb_palette_init(void)
{
	return (AFB_PALETTE) {
	.size = 0,.colors = NULL};
}

void afb_palette_free(AFB_PALETTE *pal)
{
	free(pal->colors);
}
