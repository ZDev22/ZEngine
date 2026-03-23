/* licensed under GPL v3.0 - see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v2.0.1

ztext.hpp is a lightweight cross-platform single-header cpp font-text rasterizer built off stb_truetype

HOW TO USE:
#define STB_TRUETYPE_IMPLEMENTATION - define this in one cpp file
#define ZTEXT_IMPLEMENTATION - define this in the same cpp file

*/

#pragma once

#include "zengine.hpp"

#if defined(ZTEXT_IMPLEMENTATION) && !defined(ZENGINE_DEPS_DEFINED)
    #define STB_TRUETYPE_IMPLEMENTATION
    #define STBTT_ASSERT
#endif
#include "deps/stb_truetype.h"

#ifdef ZTEXT_IMPLEMENTATION

/* thanks to https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c for (the base of) this implementation! */
std::unique_ptr<Texture> createText(const char* word) {
    /* load font */
    long size = 0;
    unsigned char* fontBuffer;

    FILE* fontFile = fopen("assets/fonts/Bullpen3D.ttf", "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    stbtt_fontinfo info;
    stbtt_InitFont(&info, fontBuffer, 0);

    int b_w = 512;
    int b_h = 128;
    int l_h = 64;

    unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h, 1);

    float scale = stbtt_ScaleForPixelHeight(&info, l_h);

    int x = 0; int ascent = 0; int descent = 0; int lineGap = 0;

    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);
 
    for (unsigned int i = 0; i < strlen(word); ++i) {
        /* characer width */
        int ax = 0; int lsb = 0;
        stbtt_GetCodepointHMetrics(&info, word[i], &ax, &lsb);

        /* get bounding box */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* get y */
        int y = ascent + c_y1;

        /* render character */
        int byteOffset = x + roundf(lsb * scale) + (y * b_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* advance & kerning */
        x += roundf(ax * scale) + roundf(stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]) * scale);
    }

    return std::make_unique<Texture>(bitmap, b_w, b_h);
}

#undef ZTEXT_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#endif
