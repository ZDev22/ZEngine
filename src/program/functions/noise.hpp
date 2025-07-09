#pragma once

#include "deps/FastNoiseLite.h"
#include <vector>

// TODO: Add FastNoise2 support (and make sure this works)
class Noise {
public:
    Noise();

    void setSeed(int& seed) { noise.SetSeed(seed); }
    void setFrequency(float& frequency) { noise.SetFrequency(frequency); }

    float whiteNoise(float& x, float& y) {
        noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
        return noise.GetNoise(x, y);
    }

    float perlinNoise(float& x, float& y) {
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        return noise.GetNoise(x, y);
    }

    float fractalBrowianMotion(float& x, float& y, int& octaves) {
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetFractalOctaves(octaves);
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        return noise.GetNoise(x, y);
    }

    float domainWarp(float& x, float& y, float& warpAmp) {
        noise.SetDomainWarpAmp(warpAmp);
        noise.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        return noise.GetNoise(x, y);
    }

    std::vector<float> generateHeightmap(int& width, int& height, float& scale, float& (Noise::*noiseFunc)(float, float), int& octaves) {
        std::vector<float> heightmap(width * height);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float nx = x * scale, ny = y * scale, value;

                if (noiseFunc == &Noise::fractalNoise) { value = (this->*noiseFunc)(nx, ny, octaves); }
                else { value = (this->*noiseFunc)(nx, ny); }

                heightmap[y * width + x] = value;
            }
        }
        return heightmap;
    }

private:
    FastNoiseLite noise;
};