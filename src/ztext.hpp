/* licensed under GPL v3.0 - see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

v1.0.0

ztext.hpp is a lightweight cross-platform single-header cpp font-text rasterizer built off stb_truetype

HOW TO USE:
#define STB_TRUETYPE_IMPLEMENTATION - define this in one cpp file
#define ZTEXT_IMPLEMENTATION - define this in the same cpp file

TODO:
 * use width x height for font size instead of being square
 * using kerning to shrink size and look better
 */

#pragma once

#include "zengine.hpp"

#include "deps/stb_truetype.h"

struct ZText {
    stbtt_bakedchar charData[96];
    std::vector<unsigned char> grayscale;
};

extern std::vector<ZText> fonts;

std::unique_ptr<Texture> createText(const unsigned int font, const std::string& text, const unsigned int fontSize);
void loadFont(const char* font, const unsigned int fontSize);

#ifdef ZTEXT_IMPLEMENTATION

std::vector<ZText> fonts;

std::unique_ptr<Texture> createText(const unsigned int font, const std::string& text, const unsigned int fontSize) {
    unsigned int atlasSize = fontSize * 16;
    float minY = 0.0f;
    float maxY = 0.0f;
    float currentX = 0.0f;
    stbtt_bakedchar cache;

    for (char c : text) {
        if (c < 32) { continue; }
        cache = fonts[font].charData[c - 32];
        minY = minY < cache.yoff ? minY : cache.yoff;
        maxY = maxY > cache.yoff + (cache.y1 - cache.y0) ? maxY : cache.yoff + (cache.y1 - cache.y0);
        currentX += cache.xadvance;
    }

    unsigned int texsize = ((unsigned int)currentX + 1 > (unsigned int)(maxY - minY) + 1 ? (unsigned int)currentX : (unsigned int)(maxY - minY)) + 1;
    std::vector<unsigned char> textGrayscale(texsize * texsize, 0);

    currentX = 0.0f;
    int xDistance = 0;
    int yDistance = 0;
    int xSource = 0;
    int ySource = 0;
    int xID = 0;
    int xDistanceID = 0;

    for (char c : text) {
        if (c < 32) { continue; }

        cache = fonts[font].charData[c - 32];
        xDistance = (int)(currentX + cache.xoff);
        yDistance = (int)(cache.yoff - minY);

        for (int posY = 0; posY < cache.y1 - cache.y0; ++posY) {
            for (int posX = 0; posX < cache.x1 - cache.x0; ++posX) {
                xSource = cache.x0 + posX;
                ySource = cache.y0 + posY;
                xID = ySource * atlasSize + xSource;
                xDistanceID = (yDistance + posY) * texsize + (xDistance + posX);

                if (xDistanceID >= 0 && xDistanceID < (int)(texsize * texsize)) { textGrayscale[xDistanceID] = fonts[font].grayscale[xID]; }
            }
        }

        currentX += cache.xadvance;
    }

    return std::make_unique<Texture>(textGrayscale.data(), texsize);
}

void loadFont(const char* font, const unsigned int fontSize) {
    FILE* file = fopen(font, "rb");
    if (!file) { throw("Failed to open font file"); }

    fseek(file, 0, SEEK_END);
    unsigned int fileSize = (unsigned int)ftell(file);
    rewind(file);

    unsigned char* buffer = new unsigned char[fileSize];
    if (fread(buffer, 1, fileSize, file) != fileSize) {
        fclose(file);
        delete[] buffer;
        throw("Failed to read font file");
    }

    fclose(file);

    ZText fontData;
    unsigned int atlasSize = fontSize * 16;
    fontData.grayscale.resize(atlasSize * atlasSize);
    stbtt_BakeFontBitmap(buffer, 0, fontSize, fontData.grayscale.data(), atlasSize, atlasSize, 32, 96, fontData.charData);
    delete[] buffer;

    fonts.emplace_back(fontData);
}

#undef ZTEXT_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#endif