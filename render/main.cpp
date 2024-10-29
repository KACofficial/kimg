#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <array>
#include <stdexcept>
#include <cstdio>
#include <cstdint>
#include <argparse/argparse.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

using Tribyte = std::array<uint8_t, 3>;
using Pixel = std::tuple<unsigned char, unsigned char, unsigned char>;

using PixelData = std::vector<Pixel>;

struct Header {
    Tribyte width;
    Tribyte height;
};

Header readKimgHeader(const char *filename) {
    Header header;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open KIMG file!" << std::endl;
        throw std::runtime_error("File not found");
    }

    // Read width and height as Tribyte
    inputFile.read(reinterpret_cast<char*>(header.width.data()), header.width.size());
    inputFile.read(reinterpret_cast<char*>(header.height.data()), header.height.size());

    inputFile.close();
    return header;
}

PixelData readKimgPixels(const char *filename, int &w, int &h) {
    Header header = readKimgHeader(filename);
    w = (header.width[0] << 16) | (header.width[1] << 8) | header.width[2];
    h = (header.height[0] << 16) | (header.height[1] << 8) | header.height[2];

    std::ifstream inputFile(filename, std::ios::binary);
    inputFile.seekg(6);  // Skip header

    PixelData pixels;
    unsigned char r, g, b;
    for (int i = 0; i < w * h; ++i) {
        inputFile.read(reinterpret_cast<char*>(&r), sizeof(r));
        inputFile.read(reinterpret_cast<char*>(&g), sizeof(g));
        inputFile.read(reinterpret_cast<char*>(&b), sizeof(b));
        pixels.emplace_back(r, g, b);
    }

    inputFile.close();
    return pixels;
}

void renderKimgToPng(const char *kimgFilename, const char *pngFilename) {
    int width, height;
    PixelData pixels = readKimgPixels(kimgFilename, width, height);

    // Prepare the data array for stb_image_write
    std::vector<unsigned char> imageData(width * height * 3);
    for (int i = 0; i < width * height; ++i) {
        unsigned char r, g, b;
        std::tie(r, g, b) = pixels[i];
        imageData[i * 3] = r;       // Red
        imageData[i * 3 + 1] = g;   // Green
        imageData[i * 3 + 2] = b;   // Blue
    }

    // Write the image data to a PNG file
    if (stbi_write_png(pngFilename, width, height, 3, imageData.data(), width * 3)) {
        std::cout << "Image successfully saved to " << pngFilename << std::endl;
    } else {
        std::cerr << "Failed to save image!" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("kconvert", "0.0.1");

    program.add_description("Convert Kimg files to other formats, like PNG.");

    program.add_argument("source")
          .help("The .kimg file to convert")
          .required();

    program.add_argument("output")
          .help("The file to output to.")
          .default_value("image.png");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    try {
        std::string source = program.get<std::string>("source");
        std::string output = program.get<std::string>("output");

        renderKimgToPng(source.c_str(), output.c_str());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
