// Suppose we have an image with an arbitrary number of pixels and want to generate a palette of 16 colors.
// Put all the pixels of the image (that is, their RGB values) in a bucket.
// Find out which color channel (red, green, or blue) among the pixels in the bucket has the greatest range,
// then sort the pixels according to that channel's values.
// For example, if the blue channel has the greatest range, then a pixel with an RGB value of (32, 8, 16) is less than a pixel with an RGB value of (1, 2, 24),
// because 16 < 24. After the bucket has been sorted, move the upper half of the pixels into a new bucket.
// (It is this step that gives the median cut algorithm its name;
// the buckets are divided into two at the median of the list of pixels.)
// This process can be repeated to further subdivide the set of pixels:
// choose a bucket to divide (e.g., the bucket with the greatest range in any color channel) and divide it into two.
// After the desired number of buckets have been produced, average the pixels in each bucket to get the final color palette.
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "color_quantization.h"
#include "img.h"

#define RED 0
#define GREEN 1
#define BLUE 2

int get_colormapped_pixel(int r, int g, int b, BUCKET **colormap, int size) 
{
    double current_squared_distance = 0;
    double previous_squared_distance = DBL_MAX;
    int smallest_distance_index = 0;
    for (int i = 0; i < size; i++) {
        current_squared_distance = pow(r - colormap[i]->red_avg, 2)
            + pow(g - colormap[i]->green_avg, 2)
            + pow(b - colormap[i]->blue_avg, 2);

        if (current_squared_distance < previous_squared_distance) {
            previous_squared_distance = current_squared_distance;
            smallest_distance_index = i;
        }
    }
    return smallest_distance_index;
}

int check_sort(uint8_t *pixels, int size, int offset)
{
    uint16_t previous_color = 0;
    for (int i = 0; i < size; i++) {
        if (previous_color > pixels[i * 3 + offset])
            return 1;
        previous_color = pixels[i * 3 + offset];
    }
    return 0;
}

void print_bucket(BUCKET *bucket)
{
    printf("BUCKET\n");
    printf("\tCOLORS:\n");
    for (int i=0; i < bucket->size; i++) {
        printf("\t\tR: %i G: %i B: %i\n", bucket->begin[i * 3 + RED], bucket->begin[i * 3 + GREEN], bucket->begin[i * 3 + BLUE]);
    }
    printf("\tBEGIN: %p\n", bucket->begin);
    printf("\tSIZE: %i\n", bucket->size);
    printf("\tLARGEST CHANNEL: %i\n", bucket->largest_channel);
    printf("\tRED AVG:\t%i\n", bucket->red_avg);
    printf("\tGREEN AVG:\t%i\n", bucket->green_avg);
    printf("\tBLUE AVG:\t%i\n", bucket->blue_avg);
}

void print_buckets(BUCKET **buckets, int size)
{
    for (int i=0; i < size; i++) {
        print_bucket(buckets[i]);
    }
}

PALETTE quantize_median_cut(IMAGE img, unsigned int palette_size)
{
    unsigned int img_size = img.height * img.width;
    unsigned int unique_col_count = 0;
    unsigned int total_buckets = 1;
    unsigned int split_bucket_size_1;
    unsigned int split_bucket_size_2;
    unsigned int r_avg, g_avg, b_avg;
    uint8_t largest_channel;
    BUCKET **bucket_list;
    PALETTE palette;
    uint8_t *unique_colors = malloc(img_size * 3);
    uint8_t *pixels = malloc(img_size * 3);
    
    memcpy(pixels, img.image_data, img_size * 3);
    
    // TODO put this in its own function
    /* Get all the unique colors from the image */
    for (int i=0; i < img_size; i++) {
        bool found = false;
        for (int j = 0; j < unique_col_count; j++) {
            if (pixels[i * 3 + RED] == unique_colors[j * 3 + RED] 
                && pixels[i * 3 + GREEN] == unique_colors[j * 3 + GREEN] 
                && pixels[i * 3 + BLUE] == unique_colors[j * 3 + BLUE]) {
                found = true;
                break;
            }
        }

        if(!found) {
            unique_colors[unique_col_count * 3 + RED] = pixels[i * 3 + RED]; 
            unique_colors[unique_col_count * 3 + GREEN] = pixels[i * 3 + GREEN]; 
            unique_colors[unique_col_count * 3 + BLUE] = pixels[i * 3 + BLUE]; 
            unique_col_count++;
        }
    }

    // TODO sizeof(*bucket_list) can probably be changed to sizeof(*BUCKET) for clarity
    /* Create a list of buckets with a size of our desired palette */
    bucket_list = malloc(sizeof(*bucket_list) * palette_size);

    /* Initialize each bucket in the bucket list*/
    for (int i=0; i < palette_size; i++) {
        bucket_list[i] = malloc(sizeof(BUCKET));
        *bucket_list[i] = (BUCKET){.size = 0, .begin = 0, .ranges = {0,0,0}, .largest_channel = 0, .red_avg = 0, .green_avg = 0, .blue_avg = 0};
    }

    bucket_list[0]->size = unique_col_count;
    bucket_list[0]->begin = unique_colors;

    sort_colors_channel(bucket_list[0]);

    for (int i = 1; i < palette_size; i++) {
        uint8_t largest_range_bucket = 0;
        int bucket_to_split = -1;

        /* Find bucket with largest range in any color channel */
        for (size_t j = 0; j < total_buckets; j++) {
            largest_channel = bucket_list[j]->ranges[bucket_list[j]->largest_channel];

            /* Skip if bucket has one pixel left */
            if(bucket_list[j]->size <= 1)
                continue;

            if(largest_channel > largest_range_bucket) {
                largest_range_bucket = largest_channel;
                bucket_to_split = j;
            }
        }

        /* No buckets found to split */
        if(bucket_to_split == -1)
            break;

        /* Assign new data to bucket */
        split_bucket_size_1 = bucket_list[bucket_to_split]->size >> 1;
        split_bucket_size_2 = bucket_list[bucket_to_split]->size - split_bucket_size_1;

        bucket_list[total_buckets]->size = split_bucket_size_2;
        bucket_list[total_buckets]->begin = &bucket_list[bucket_to_split]->begin[split_bucket_size_1 * 3];

        /* Split the current bucket */
        bucket_list[bucket_to_split]->size = split_bucket_size_1;

        /* Reorder buckets */
        sort_colors_channel(bucket_list[bucket_to_split]); // Resort old resized bucket
        sort_colors_channel(bucket_list[total_buckets]); // New bucket

        total_buckets++;
    }

    /* Finally, calculate the average color of every bucket */
    palette.size = palette_size;
    palette.colors = malloc(sizeof(COLOR) * palette.size);
    for (int i=0; i < total_buckets; i++) {
        r_avg = g_avg = b_avg = 0;

        for (size_t j = 0; j < bucket_list[i]->size; j++) {
            r_avg += bucket_list[i]->begin[j * 3 + RED];
            g_avg += bucket_list[i]->begin[j * 3 + GREEN];
            b_avg += bucket_list[i]->begin[j * 3 + BLUE];
        }
        r_avg /= bucket_list[i]->size;
        g_avg /= bucket_list[i]->size;
        b_avg /= bucket_list[i]->size;

        palette.colors[i].red = (uint8_t)r_avg;
        palette.colors[i].green = (uint8_t)g_avg;
        palette.colors[i].blue = (uint8_t)b_avg;
    }

    /* Be free! */
    for (int i=0; i < total_buckets; i++) {
        free(bucket_list[i]);
    }
    free(bucket_list);
    free(unique_colors);
    free(pixels);
    
    return palette;
}

void sort_colors_channel(BUCKET *bucket)
{
    uint16_t minR = 255;
    uint16_t maxR = 0;
    uint16_t minG = 255;
    uint16_t maxG = 0;
    uint16_t minB = 255;
    uint16_t maxB = 0;

    /* Go through all the values in a bucket and find the smallest and the
       biggest value for each channel */
    for (size_t i = 0; i < bucket->size; i++)
    {
        minR = bucket->begin[i * 3 + RED] < minR   ? bucket->begin[i * 3 + RED] : minR;
        maxR = bucket->begin[i * 3 + RED] > maxR   ? bucket->begin[i * 3 + RED] : maxR;
        minG = bucket->begin[i * 3 + GREEN] < minG ? bucket->begin[i * 3 + GREEN] : minG;
        maxG = bucket->begin[i * 3 + GREEN] > maxG ? bucket->begin[i * 3 + GREEN] : maxG;
        minB = bucket->begin[i * 3 + BLUE] < minB  ? bucket->begin[i * 3 + BLUE] : minB;
        maxB = bucket->begin[i * 3 + BLUE] > maxB  ? bucket->begin[i * 3 + BLUE] : maxB;
    }

    /* Calculate ranges between the smallest and biggest value of each channel
     */
    int r_range = bucket->ranges[RED] = maxR - minR;
    int g_range =  bucket->ranges[GREEN] = maxG - minG;
    int b_range =  bucket->ranges[BLUE] = maxB - minB;

    /* Find which channel has the biggest range */
    int col_offset = bucket->largest_channel = RED;
    if(g_range >= r_range && g_range >= b_range)
        col_offset = bucket->largest_channel = GREEN;
    if(b_range >= r_range && b_range >= g_range)
        col_offset = bucket->largest_channel = BLUE;

    /* Sort the bucket with bubble sort */
    while(check_sort(bucket->begin, bucket->size, col_offset)) {
        uint8_t previous_pixel[] = {0,0,0};
        for (int i = 0; i < bucket->size; i++) {
            if (previous_pixel[col_offset] > bucket->begin[i * 3 + col_offset]) {
                bucket->begin[(i - 1) * 3 + RED] =   bucket->begin[i * 3 + RED];
                bucket->begin[(i - 1) * 3 + GREEN] = bucket->begin[i * 3 + GREEN];
                bucket->begin[(i - 1) * 3 + BLUE] =  bucket->begin[i * 3 + BLUE];

                bucket->begin[i * 3 + RED] =   previous_pixel[RED];
                bucket->begin[i * 3 + GREEN] = previous_pixel[GREEN];
                bucket->begin[i * 3 + BLUE] =  previous_pixel[BLUE];
            }
            previous_pixel[RED] =   bucket->begin[i * 3 + RED];
            previous_pixel[GREEN] = bucket->begin[i * 3 + GREEN];
            previous_pixel[BLUE] =  bucket->begin[i * 3 + BLUE];
        }
    }
}
