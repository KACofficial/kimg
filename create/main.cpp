#include <algorithm>
#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "main.hpp"

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("convertk", "0.0.1");

  program.add_description("Convert Kimg files to other formats, like PNG.");

  program.add_argument("source")
      .help("The image file to convert from")
      .required();

  program.add_argument("output")
      .help("The kimg file to output to")
      .default_value("image.kimg");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  std::string source = program.get<std::string>("source");
  std::string output = program.get<std::string>("output");

  image img;

  unsigned char *imgData =
      stbi_load(source.c_str(), &img.w, &img.h, &img.ch, 0);

  if (!imgData) {
    std::cerr << "Failed to load image!" << std::endl;
    return 1;
  }

  if (img.ch < 3) {
    std::cerr << "Image must be full RGB!" << std::endl;
    stbi_image_free(imgData);
    return 1;
  }

  convertToKimg(output.c_str(), img.w, img.h, img.ch, imgData);

  stbi_image_free(imgData);
  return 0;
}

PixelData getPixelData(int w, int h, int channels, unsigned char *data) {
  PixelData pixelVals;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      unsigned char *pixel = data + (y * w + x) * channels;
      unsigned char r = pixel[0];
      unsigned char g = pixel[1];
      unsigned char b = pixel[2];

      pixelVals.emplace_back(r, g, b);
    }
  }

  return pixelVals;
}

void writeToBin(const char *filename, unsigned char *data, PixelData pixels) {
  std::ofstream outFile(filename, std::ios::binary);
  if (!outFile) {
    std::cerr << "Failed to open binary file for writing!" << std::endl;
    stbi_image_free(data);
    return;
  }

  for (const auto &pixel : pixels) {
    unsigned char r, g, b;
    std::tie(r, g, b) = pixel;
    outFile.write(reinterpret_cast<const char *>(&r), sizeof(r));
    outFile.write(reinterpret_cast<const char *>(&g), sizeof(g));
    outFile.write(reinterpret_cast<const char *>(&b), sizeof(b));
  }

  outFile.close();
}

bool writeImage(const char *filename, int w, int h, int channels,
                unsigned char *data) {
  std::string ext =
      std::string(filename).substr(std::string(filename).find_last_of(".") + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (ext == "png") {
    return stbi_write_png(filename, w, h, channels, data, w * channels) != 0;
  }
  if (ext == "jpg" || ext == "jpeg") {
    return stbi_write_jpg(filename, w, h, channels, data, 100) != 0;
  }
  if (ext == "bmp") {
    return stbi_write_bmp(filename, w, h, channels, data) != 0;
  }
  if (ext == "tga") {
    return stbi_write_tga(filename, w, h, channels, data) != 0;
  }
  return false;
}

// bool convertToKimg(const char *filename, int w, int h, int channels,
//                    unsigned char *data) {
//   Header KimgHeader;
//   PixelData KimgPixels;
//   Tribyte wBytes = convertIntToTribyte(w);
//   Tribyte hBytes = convertIntToTribyte(h);
//   std::get<0>(KimgHeader) = wBytes;
//   std::get<1>(KimgHeader) = hBytes;

//   for (int y = 0; y < h; y++) {
//     for (int x = 0; x < w; x++) {
//       unsigned char *pixel = data + (y * w + x) * channels;
//       unsigned char r = pixel[0];
//       unsigned char g = pixel[1];
//       unsigned char b = pixel[2];

//       KimgPixels.emplace_back(r, g, b);
//     }
//   }

//   std::ofstream outputFile(filename, std::ios::binary);

//   // Write the header
//   outputFile.write(
//       reinterpret_cast<const char *>(std::get<0>(KimgHeader).data()),
//       std::get<0>(KimgHeader).size());
//   outputFile.write(
//       reinterpret_cast<const char *>(std::get<1>(KimgHeader).data()),
//       std::get<1>(KimgHeader).size());

//   // Write the pixels
//   for (int y = 0; y < h; y++) {
//     for (int x = 0; x < w; x++) {
//       unsigned char *pixel = data + (y * w + x) * channels;
//       outputFile.write(reinterpret_cast<const char *>(pixel), channels);
//     }
//   }

//   outputFile.close();

//   return true;
// }
bool convertToKimg(const char *filename, int w, int h, int channels,
                   unsigned char *data) {
  if (channels < 3) {
    std::cerr << "Image must have at least 3 channels (RGB)!" << std::endl;
    return false;
  }

  Header KimgHeader;
  Tribyte wBytes = convertIntToTribyte(w);
  Tribyte hBytes = convertIntToTribyte(h);

  std::get<0>(KimgHeader) = wBytes;
  std::get<1>(KimgHeader) = hBytes;

  std::ofstream outputFile(filename, std::ios::binary);
  if (!outputFile) {
    std::cerr << "Failed to open KIMG file for writing!" << std::endl;
    return false;
  }

  // Write the header
  outputFile.write(
      reinterpret_cast<const char *>(std::get<0>(KimgHeader).data()),
      std::get<0>(KimgHeader).size());
  outputFile.write(
      reinterpret_cast<const char *>(std::get<1>(KimgHeader).data()),
      std::get<1>(KimgHeader).size());

  // Write the pixel data
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      unsigned char *pixel = data + (y * w + x) * channels;
      // Only write RGB (3 channels)
      if (channels >= 3) {
        outputFile.write(reinterpret_cast<const char *>(pixel),
                         3); // Only writing RGB
      } else {
          std::cout << "Failed writing: (" << x << ", " << y << ")" << std::endl;
      }
    }
  }

  outputFile.close();
  return true;
}
Tribyte convertIntToTribyte(int value) {
  Tribyte bytes;
  bytes[0] = (value >> 16) & 0xFF;
  bytes[1] = (value >> 8) & 0xFF;
  bytes[2] = value & 0xFF;
  return bytes;
}

void printTribyteArray(Tribyte &bytes) {
  for (uint8_t byte : bytes) {
    printf("%02X ", byte);
  }
  std::cout << std::endl;
}

unsigned char intToByte(int value) {
  if (value < 0 || value > 255) {
    throw std::out_of_range("Value must be in the range of 0-255.");
  }
  return static_cast<unsigned char>(value);
}
