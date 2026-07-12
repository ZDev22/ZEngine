/* licensed under GPL v3.0 - see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license
thanks to https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c for (the base of) this implementation!

v4.2.2

ztext.h is a lightweight cross-platform single-header c font-text rasterizer built off stb_truetype
it is recommended to set stbi_write_png_compression_level = 0

HOW TO USE:
#define STB_TRUETYPE_IMPLEMENTATION - define this in one c file
#define STB_IMAGE_WRITE_IMPLEMENTATION - define this in one c file
#define ZTEXT_IMPLEMENTATION - define this in the same c file
#define ZTEXT_MAX_FONTS 5 - define this to decide how many max fonts to load (max: 255)
*/

#ifndef ZTEXT_H
#define ZTEXT_H

#include "zengine.h"

void loadFont(const char* font, const unsigned char index);
void createText(const char* word, const size_t width, const size_t height, const unsigned int resolution, const unsigned char fontIndex, const unsigned int textureIndex);

#ifdef ZTEXT_IMPLEMENTATION

#ifndef ZTEXT_MAX_FONTS
    #define ZTEXT_MAX_FONTS 1
#endif

#include "deps/stb_truetype.h"

stbtt_fontinfo fonts[ZTEXT_MAX_FONTS];

void loadFont(const char* font, const unsigned char index) {
    FILE* fontFile = fopen(font, "rb");
    fseek(fontFile, 0, SEEK_END);
    unsigned int size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    stbtt_InitFont(&fonts[index], fontBuffer, 0);
}

void createText(const char* word, const size_t width, const size_t height, const unsigned int resolution, const unsigned char fontIndex, const unsigned int textureIndex) {
    unsigned char* bitmap = (unsigned char*)calloc(1, width * height);

    float scale = stbtt_ScaleForPixelHeight(&fonts[fontIndex], resolution);

    unsigned int x = 0;
    int ascent  = 0;
    int descent = 0;
    int lineGap = 0;

    stbtt_GetFontVMetrics(&fonts[fontIndex], &ascent, &descent, &lineGap);

    ascent = (int)((float)ascent * scale);
    descent = (int)((float)descent * scale);

    for (unsigned int i = 0; i < strlen(word); ++i) {
        /* char width */
        int ax = 0; int lsb = 0;
        stbtt_GetCodepointHMetrics(&fonts[fontIndex], word[i], &ax, &lsb);

        /* get bounding box */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&fonts[fontIndex], word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* render char */
        int byteOffset = x + (int)((float)lsb * scale) + ((ascent + c_y1) * width);
        stbtt_MakeCodepointBitmap(&fonts[fontIndex], bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, word[i]);

        /* advance & kerning */
        x += (unsigned int)((float)ax * scale) + (int)((float)stbtt_GetCodepointKernAdvance(&fonts[fontIndex], word[i], word[i + 1]) * scale);
    }

    unsigned int imageSize = width * height * 4;
    unsigned char* pixels = (unsigned char*)malloc(imageSize);
    unsigned int j = 0;
    for (unsigned int i = 0; i < imageSize; i += 4) {
        pixels[i] = bitmap[j];
        pixels[i + 1] = bitmap[j];
        pixels[i + 2] = bitmap[j];
        pixels[i + 3] = bitmap[j];
        j++;
    }

    createTextureExt(pixels, textureIndex, width, height, imageSize, VK_FORMAT_R8G8B8A8_SRGB);
    free(bitmap);
    free(pixels);
}

#endif // ZTEXT_IMPLEMENTATION
#endif // ZTEXT_H

