/* licensed under GPL v3.0 - see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v3.1.1

ztext.h is a lightweight cross-platform single-header c font-text rasterizer built off stb_truetype

HOW TO USE:
#define STB_TRUETYPE_IMPLEMENTATION - define this in one c file
#define ZTEXT_IMPLEMENTATION - define this in the same c file

*/

#ifndef ZTEXT_H
#define ZTEXT_H

#include "zengine.h"

void loadFont(const char* font);
Texture* createTextPtr(const char* word, unsigned char index, const unsigned int l_h, const unsigned int b_w, const unsigned int b_h);
Texture* createText(const char* word, unsigned char index, const unsigned int l_h);

#ifdef ZTEXT_IMPLEMENTATION

#include "deps/stb_truetype.h"

stbtt_fontinfo* fonts = NULL;
unsigned short fontSize = 0;

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

    stbtt_fontinfo* oldInfo = (stbtt_fontinfo*)malloc(fontSize * sizeof(stbtt_fontinfo));
    memcpy(oldInfo, fonts, fontSize * sizeof(stbtt_fontinfo));
    
    free(fonts);
    fonts = (stbtt_fontinfo*)malloc((fontSize + 1) * sizeof(stbtt_fontinfo));
    memcpy(fonts, oldInfo, fontSize * sizeof(stbtt_fontinfo));
    fonts[fontSize] = info;
    fontSize++;
}

/* thanks to https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c for (the base of) this implementation! */
Texture* createTextPtr(const char* word, unsigned char index, const unsigned int l_h, const unsigned int b_w, const unsigned int b_h) {
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
        /* char width */
        int ax = 0; int lsb = 0;
        stbtt_GetCodepointHMetrics(&fonts[index], word[i], &ax, &lsb);

        /* get bounding box */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&fonts[index], word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

        /* render char */
        int byteOffset = x + (int)((float)lsb * scale) + ((ascent + c_y1) * b_w);
        stbtt_MakeCodepointBitmap(&fonts[index], bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

        /* advance & kerning */
        x += (unsigned int)((float)ax * scale) + (int)((float)stbtt_GetCodepointKernAdvance(&fonts[index], word[i], word[i + 1]) * scale);
    }

    Texture* output = (Texture*)malloc(sizeof(Texture));
    createTextureFromData(output, bitmap, b_w, b_h);
    return output;
}

Texture* createText(const char* word, unsigned char index, const unsigned int l_h) { return createTextPtr(word, index, l_h, 512, 256); }

#endif // ZTEXT_IMPLEMENTATION
#endif // ZTEXT_H

