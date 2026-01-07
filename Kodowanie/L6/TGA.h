#ifndef TGA_H
#define TGA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#pragma pack(push, 1)
struct TGAHeader {
    uint8_t  idLength;
    uint8_t  colorMapType;
    uint8_t  imageType;
    uint16_t colorMapOrigin;
    uint16_t colorMapLength;
    uint8_t  colorMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t  bitsPerPixel;
    uint8_t  imageDescriptor;
};
#pragma pack(pop)

struct Pixel {
    uint8_t b, g, r;
};

#endif
