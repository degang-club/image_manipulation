#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "color_quantization.h"
#include "img.h"

#define RED 0
#define GREEN 1
#define BLUE 2

int check_sort(uint32_t *pixels, int size, uint32_t mask)
{
    uint32_t previous_color = 0;
    for (int i = 0; i < size; i++) {
        if ((previous_color & mask) > (pixels[i] & mask))
            return 1;
        previous_color = pixels[i];
    }
    return 0;
}

/* Get all the unique colors from the image */
PALETTE afb_unique_colors(uint8_t *img_data, unsigned int img_size)
{
    PALETTE pal = afb_palette_init();
    uint32_t col;
    bool found;
    
    pal.colors = malloc(img_size * sizeof(uint32_t));

    /* Loop through the entire image */
    for (int i=0; i < img_size; i++) {
        // col = get_col(img_data, i);
        col = afb_to_rgba(img_data[i * 3], img_data[i * 3 + 1], img_data[i * 3 + 2], 0);
        found = false;
        /* Compare against all other colors in the palette*/
        for (int j = 0; j < pal.size; j++) {
            if (col == pal.colors[j]) {
                found = true;
                break;
            }
        }

        /* Add to the palette if not found*/
        if(!found) {
            pal.colors[pal.size] = col;
            pal.size++;
        }
    }

    pal.colors = realloc(pal.colors, pal.size * sizeof(uint32_t));
    return pal;
}

PALETTE quantize_median_cut(IMAGE img, unsigned int palette_size)
{
    unsigned int img_size = img.height * img.width;
    unsigned int total_buckets = 1;
    unsigned int split_bucket_size_1;
    unsigned int split_bucket_size_2;
    unsigned int r_avg, g_avg, b_avg;
    uint8_t largest_channel;
    BUCKET **bucket_list;
    PALETTE pal_w = afb_unique_colors(img.image_data, img_size); // working palette
    PALETTE pal_f = afb_palette_init(); // final palette
    
    // TODO sizeof(*bucket_list) can probably be changed to sizeof(*BUCKET) for clarity
    /* Create a list of buckets with a size of our desired palette */
    bucket_list = malloc(sizeof(*bucket_list) * palette_size);

    /* Initialize each bucket in the bucket list*/
    for (int i=0; i < palette_size; i++) {
        bucket_list[i] = malloc(sizeof(BUCKET));
        *bucket_list[i] = (BUCKET){.palette.size = 0, .palette.colors = 0, .ranges = {0,0,0}, .largest_channel = 0};
    }

    bucket_list[0]->palette.size = pal_w.size;
    bucket_list[0]->palette.colors = pal_w.colors;

    sort_colors_channel(bucket_list[0]);

    for (int i = 1; i < palette_size; i++) {
        uint8_t largest_range_bucket = 0;
        int bucket_to_split = -1;

        /* Find bucket with largest range in any color channel */
        for (size_t j = 0; j < total_buckets; j++) {
            largest_channel = bucket_list[j]->ranges[bucket_list[j]->largest_channel];

            /* Skip if bucket has one pixel left */
            if(bucket_list[j]->palette.size <= 1)
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
        split_bucket_size_1 = bucket_list[bucket_to_split]->palette.size >> 1; // divide by two
        split_bucket_size_2 = bucket_list[bucket_to_split]->palette.size - split_bucket_size_1;

        bucket_list[total_buckets]->palette.size = split_bucket_size_2;
        bucket_list[total_buckets]->palette.colors = &bucket_list[bucket_to_split]->palette.colors[split_bucket_size_1];

        /* Split the current bucket */
        bucket_list[bucket_to_split]->palette.size = split_bucket_size_1;

        /* Reorder buckets */
        sort_colors_channel(bucket_list[bucket_to_split]); // Sort old resized bucket
        sort_colors_channel(bucket_list[total_buckets]); // New bucket

        total_buckets++;
    }

    /* Finally, calculate the average color of every bucket */
    pal_f.size = palette_size;
    pal_f.colors = malloc(sizeof(uint32_t) * pal_f.size);
    for (int i=0; i < total_buckets; i++) {
        r_avg = g_avg = b_avg = 0;

        for (size_t j = 0; j < bucket_list[i]->palette.size; j++) {
            r_avg += afb_rgba_get_r(bucket_list[i]->palette.colors[j]);
            g_avg += afb_rgba_get_g(bucket_list[i]->palette.colors[j]);
            b_avg += afb_rgba_get_b(bucket_list[i]->palette.colors[j]);
        }
        r_avg /= bucket_list[i]->palette.size;
        g_avg /= bucket_list[i]->palette.size;
        b_avg /= bucket_list[i]->palette.size;

        pal_f.colors[i] = afb_to_rgba(r_avg, g_avg, b_avg, 0);
    }

    /* Be free! */
    for (int i=0; i < total_buckets; i++) {
        free(bucket_list[i]);
    }
    free(bucket_list);
    free(pal_w.colors);
    
    return pal_f;
}

void sort_colors_channel(BUCKET *bucket)
{
    uint16_t minR = 255;
    uint16_t maxR = 0;
    uint16_t minG = 255;
    uint16_t maxG = 0;
    uint16_t minB = 255;
    uint16_t maxB = 0;
    uint32_t previous_pixel = 0;

    /* Go through all the values in a bucket and find the smallest and the
       biggest value for each channel */
    for (size_t i = 0; i < bucket->palette.size; i++)
    {
        minR = afb_rgba_get_r(bucket->palette.colors[i]) < minR ? afb_rgba_get_r(bucket->palette.colors[i]) : minR;
        maxR = afb_rgba_get_r(bucket->palette.colors[i]) > maxR ? afb_rgba_get_r(bucket->palette.colors[i]) : maxR;
        minG = afb_rgba_get_g(bucket->palette.colors[i]) < minG ? afb_rgba_get_g(bucket->palette.colors[i]) : minG;
        maxG = afb_rgba_get_g(bucket->palette.colors[i]) > maxG ? afb_rgba_get_g(bucket->palette.colors[i]) : maxG;
        minB = afb_rgba_get_b(bucket->palette.colors[i]) < minB ? afb_rgba_get_b(bucket->palette.colors[i]) : minB;
        maxB = afb_rgba_get_b(bucket->palette.colors[i]) > maxB ? afb_rgba_get_b(bucket->palette.colors[i]) : maxB;
    }

    /* Calculate ranges between the smallest and biggest value of each channel
     */
    int r_range = bucket->ranges[RED] = maxR - minR;
    int g_range =  bucket->ranges[GREEN] = maxG - minG;
    int b_range =  bucket->ranges[BLUE] = maxB - minB;

    /* Find which channel has the biggest range */
    int col_mask = 0xff000000;
    bucket->largest_channel = RED;
    if(g_range >= r_range && g_range >= b_range) {
        col_mask = 0x00ff0000;
        bucket->largest_channel = GREEN;
    }
    if(b_range >= r_range && b_range >= g_range) {
        col_mask = 0x0000ff00;
        bucket->largest_channel = BLUE;
    }

    /* Sort the bucket with bubble sort */
    while(check_sort(bucket->palette.colors, bucket->palette.size, col_mask)) {
        previous_pixel = 0;
        for (int i = 0; i < bucket->palette.size; i++) {
            if ((previous_pixel & col_mask) > (bucket->palette.colors[i] & col_mask)) {
                bucket->palette.colors[(i - 1)] = bucket->palette.colors[i];
                bucket->palette.colors[i] =   previous_pixel;
            }
            previous_pixel = bucket->palette.colors[i];
        }
    }
}
