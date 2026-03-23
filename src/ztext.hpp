/* licensed under GPL v3.0 - see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v2.0.1

ztext.hpp is a lightweight cross-platform single-header cpp font-text rasterizer built off stb_truetype

HOW TO USE:
#define STB_TRUETYPE_IMPLEMENTATION - define this in one cpp file
#define ZTEXT_IMPLEMENTATION - define this in the same cpp file

*/

#pragma once

#include "zengine.hpp"

#include "deps/stb_truetype.h"

extern std::vector<stbtt_fontinfo> fonts;

void loadFont(const char* font);
std::unique_ptr<Texture> createText(const char* word);

#ifdef ZTEXT_IMPLEMENTATION

std::vector<stbtt_fontinfo> fonts;

void loadFont(const char* font) {
    FILE* fontFile = fopen(font, "rb");
    fseek(fontFile, 0, SEEK_END);
    unsigned int size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    stbtt_fontinfo info;
    stbtt_InitFont(&info, fontBuffer, 0);

    fonts.push_back(info);
}

/* thanks to https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c for (the base of) this implementation! */
std::unique_ptr<Texture> createText(const char* word, unsigned char index, const unsigned int l_h, const unsigned int b_w = 512, const unsigned int b_h = 128) {
    unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h, 1);

    float scale = stbtt_ScaleForPixelHeight(&fonts[index], l_h);

    unsigned int x = 0;
    int ascent  = 0;
    int descent = 0;
    int lineGap = 0;

    stbtt_GetFontVMetrics(&fonts[index], &ascent, &descent, &lineGap);

    ascent = (int)((float)ascent * scale);
    descent = (int)((float)descent * scale);
 
    for (unsigned int i = 0; i < strlen(word); ++i) {
        /* characer width */
        int ax = 0; int lsb = 0;
        stbtt_GetCodepointHMetrics(&fonts[index], word[i], &ax, &lsb);

        /* get bounding box */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&fonts[index], word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* get y */
        int y = ascent + c_y1;

        /* render character */
        int byteOffset = x + (int)((float)lsb * scale) + (y * b_w);
        stbtt_MakeCodepointBitmap(&fonts[index], bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* advance & kerning */
        x += (unsigned int)((float)ax * scale) + (int)((float)stbtt_GetCodepointKernAdvance(&fonts[index], word[i], word[i + 1]) * scale);
    }

    return std::make_unique<Texture>(bitmap, b_w, b_h);
}

#undef ZTEXT_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#endif
