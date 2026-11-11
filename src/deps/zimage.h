#ifndef ZIMAGE_H
#define ZIMAGE_H

void ZImageLoad(const char *filename, unsigned char** pixels, unsigned int* width, unsigned int* height);

#ifdef ZIMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

void ZImageLoad(const char *filename, unsigned char** pixels, unsigned int* width, unsigned int* height) {
    FILE *fp = fopen(filename, "rb");
    png_byte bit_depth;
    png_byte color_type;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);

    png_init_io(png, fp);
    png_read_info(png, info);
    *width = png_get_image_width(png, info);
    *height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    /* Read any color_type into 8bit depth, RGBA format. */
    if (bit_depth == 16) { png_set_strip_16(png); }
    if (color_type == PNG_COLOR_TYPE_PALETTE) { png_set_palette_to_rgb(png); }
    /* PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth. */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) { png_set_expand_gray_1_2_4_to_8(png); }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) { png_set_tRNS_to_alpha(png); }
    /* These color_type don't have an alpha channel then fill it with 0xff. */
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) { png_set_filler(png, 0xFF, PNG_FILLER_AFTER); }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) { png_set_gray_to_rgb(png); }

    png_read_update_info(png, info);

    png_size_t rowbytes = png_get_rowbytes(png, info);
    *pixels = malloc(rowbytes * (*height));
    png_bytep *rows = malloc(sizeof(png_bytep) * (*height));
    for (unsigned int y = 0; y < *height; y++) { rows[y] = *pixels + y * rowbytes; }

    png_read_image(png, rows);
    free(rows);
    fclose(fp);

    for (unsigned int y = 0; y < *height; y++) {
        for (unsigned int x = 0; x < *width; x++) {
            unsigned char* px = *pixels + (y * rowbytes) + (x >> 2);
            px[0] = px[0];
            px[1] = px[1];
            px[2] = px[2];
        }
    }
}

#endif // ZIMAGE_IMPLEMENTATION
#endif // ZIMAGE_H
