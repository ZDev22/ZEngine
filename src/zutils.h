/* zutils.h is a single header utility for ZEngine
 * it includes decoding PNG's, converting mouse position into valid positions and fetching the window size when the window updates, and telling ZEngine that the frame buffer was resized
 *
 * #define ZUTILS_MAX_FONTS 5 - the maximum amount of fonts you can load at a time (default: 1)
 */

#ifndef ZUTILS_H
#define ZUTILS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "zengine.h"

typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned char* pixels;
} ZUtilsPNG;

typedef struct {
    ZUtilsPNG* image;
    unsigned char typeface;
} ZUtilsFont;

/* ZUtils essentials */
void ZUtilsInit(void); // initialize ZUtils
void ZUtilsTick(void); // tick ZUtils
void ZUtilsDeinit(void); // deinit ZUtils

/* texture creation */
ZUtilsPNG* ZUtilsLoadPNG(const char* filepath); // load a png from a file
void createPNGTexture(const char* filepath, unsigned int index); // create a texture from a png
void loadFont(const char* filepath, unsigned char typeface, unsigned char index); // load fonts
void createText(const char* text, unsigned char textureIndex, unsigned int fontIndex); // create a texture based on text and font

extern float mousepos[2];
extern i32 mouseposint[2];
extern i32 windowsize[2];

#ifdef ZUTILS_IMPLEMENTATION

#ifndef ZUTILS_MAX_FONTS
#define ZUTILS_MAX_FONTS 1
#endif

#include <png.h>

float mousepos[2] = {0.f};
i32 mouseposint[2] = {0};
i32 windowsize[2] = {0};

ZUtilsFont zutilsFont[ZUTILS_MAX_FONTS] = {{NULL, 0}};

/* helper funcs */
void ZUtils_UpdateMouse(void) {
    RGFW_window_getMouse(zwindow, &mouseposint[0], &mouseposint[1]);
    mousepos[0] = ((((float)mouseposint[0] / windowsize[0]) * 2.f) - 1.f) * camera.aspect;
    mousepos[1] = (((float)mouseposint[1] / windowsize[1]) * 2.f) - 1.f;
}

/* ZUTIL */
void ZUtilsInit(void) {
    ZUtils_UpdateMouse();
    RGFW_window_getSize(zwindow, &windowsize[0], &windowsize[1]);
}

ZUtilsPNG* ZUtilsLoadPNG(const char* filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) { return NULL; }
    png_byte bit_depth;
    png_byte color_type;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);

    png_init_io(png, fp);
    png_read_info(png, info);
    ZUtilsPNG* PNG = (ZUtilsPNG*)malloc(sizeof(ZUtilsPNG));
    PNG->width = png_get_image_width(png, info);
    PNG->height = png_get_image_height(png, info);
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
    PNG->pixels = (unsigned char*)malloc(rowbytes * PNG->height);
    png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * (PNG->height));
    for (unsigned int y = 0; y < PNG->height; y++) { rows[y] = PNG->pixels + y * rowbytes; }

    png_read_image(png, rows);
    png_destroy_read_struct(&png, &info, NULL);
    free(rows);
    fclose(fp);
    return PNG;
}

void createPNGTexture(const char* filepath, unsigned int index) {
    ZUtilsPNG* PNG = ZUtilsLoadPNG(filepath);
    if (PNG != NULL) {
        createTexture(PNG->pixels, index, PNG->width, PNG->height, VK_FORMAT_R8G8B8A8_SRGB);
        free(PNG->pixels);
        free(PNG);
    }
}

void loadFont(const char* filepath, unsigned char typeface, unsigned char index) {
    if (zutilsFont[index].image != NULL) {
        free(zutilsFont[index].image->pixels);
        free(zutilsFont[index].image);
    }

    zutilsFont[index].image = ZUtilsLoadPNG(filepath);
    zutilsFont[index].typeface = typeface;
}

void drawLetter(ZUtilsPNG* PNG, unsigned short targetGlyph, unsigned short glyph, unsigned char fontIndex) {
    for (unsigned char y = 0; y < zutilsFont[fontIndex].typeface; ++y) {
        memcpy(PNG->pixels + (y * PNG->width + targetGlyph * zutilsFont[fontIndex].typeface) * 4, zutilsFont[fontIndex].image->pixels + (y * zutilsFont[fontIndex].image->width + glyph * zutilsFont[fontIndex].typeface) * 4, zutilsFont[fontIndex].typeface * 4);
    }
}

void createText(const char* text, unsigned char textureIndex, unsigned int fontIndex) {
    /* size is how many characters are in the text.
     * zutilsFont[fontIndex].typeface is how many pixels the  (square) letters are */
    unsigned short size = strlen(text);

    ZUtilsPNG PNG;
    /* the total width of the finished texture (the sizex of each character * how many characters there are)*/
    PNG.width = zutilsFont[fontIndex].typeface * size;
    /* there is only 1 row of characters, so the total height of the image will be the height of a single character */
    PNG.height = zutilsFont[fontIndex].typeface;
    /* total pixels is imageSize * colors */
    PNG.pixels = (unsigned char*)calloc(PNG.width * PNG.height, 4);

    /* loop through and parse every letter */
    for (unsigned int i = 0; i < size; i++) {
        if ((unsigned char)text[i] >= 'a' && (unsigned char)text[i] <= 'z') {  drawLetter(&PNG, i, (unsigned char)text[i] - 'a', fontIndex); }
        else if ((unsigned char)text[i] >= 'A' && (unsigned char)text[i] <= 'Z') { drawLetter(&PNG, i, (unsigned char)text[i] - 'A' + 26, fontIndex); }
        else { drawLetter(&PNG, i, (unsigned char)text[i] - '0' + 52, fontIndex); }
    }

    createTexture(PNG.pixels, textureIndex, PNG.width, PNG.height, VK_FORMAT_R8G8B8A8_SRGB);
    free(PNG.pixels);
}

void ZUtilsTick(void) {
    /* poll window events */
    RGFW_event event;
    while (RGFW_window_checkEvent(zwindow, &event)) {
        switch (event.type) {
        case RGFW_windowResized:
            RGFW_window_getSize(zwindow, &windowsize[0], &windowsize[1]);
            ZEngine_framebufferResized = 1;
            break;
        case RGFW_mouseMotion:
            ZUtils_UpdateMouse();
            break;
        }
    }
}

void ZUtilsDeinit(void) {
    for (unsigned char i = 0; i < ZUTILS_MAX_FONTS; i++) {
        if (zutilsFont[i].image != NULL) {
            free(zutilsFont[i].image->pixels);
            free(zutilsFont[i].image);
        }
    }
}

#endif // ZUTILS_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

#endif // ZUTILS_H
