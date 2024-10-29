
#ifndef MAIN_HPP
#define MAIN_HPP

#include <tuple>
#include <vector>
#include <cstdint>
#include <array>

struct image {
  int w;
  int h;
  int ch;
};

using Tribyte = std::array<uint8_t, 3>;
using Pixel = std::tuple<unsigned char, unsigned char, unsigned char>;
using Header = std::tuple<Tribyte, Tribyte>;
using PixelData = std::vector<Pixel>;
// using KimgData = std::vector<Pixel>;

PixelData getPixelData(int w, int h, int channels, unsigned char *data);
void writeToBin(const char* filename, unsigned char *data, PixelData pixels);
bool writeImage(const char* filename, int w, int h, int channels, unsigned char *data);

bool convertToKimg(const char* filename, int w, int h, int channels, unsigned char *data);
Tribyte convertIntToTribyte(int value);
void printTribyteArray(Tribyte &bytes);
unsigned char intToByte(int value);

#endif
