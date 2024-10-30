#ifndef MAIN_HPP
#define MAIN_HPP

#include <vector>
#include <tuple>
#include <array>
#include <cstdio>
#include <cstdint>

using Tribyte = std::array<uint8_t, 3>;
using Pixel = std::tuple<unsigned char, unsigned char, unsigned char>;

using PixelData = std::vector<Pixel>;

struct Header {
    Tribyte width;
    Tribyte height;
};

Header readKimgHeader(const char *filename);

PixelData readKimgPixels(const char *filename, int &w, int &h);

void renderKimgToPng(const char *kimgFilename, const char *pngFilename);

#endif
