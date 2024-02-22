#pragma once

#include <vector>
#include <string>

namespace image::helper {

struct TargetImageParameters 
{
    TargetImageParameters() = default;
    TargetImageParameters(unsigned short _width, unsigned short _height, bool _flip_verticaly, bool _flip_horizontaly);

    unsigned short width;
    unsigned short height;
    bool flip_verticaly;
    bool flip_horizontaly;
};

enum class EImageFormat 
{
    JPEG, 
    PNG
};

std::vector<unsigned char> loadRawImage(const std::string &filename);

std::vector<unsigned char> prepareImageForDeck(std::vector<unsigned char> &image_file_raw_data, const TargetImageParameters &image_params);

std::vector<unsigned char> prepareImageForDeck(const std::string &filename, const TargetImageParameters &image_params);

std::vector<unsigned char> applyLabelOnImage(const std::vector<unsigned char> &image_file_raw_data, const std::string &label, int font_size = 12);

std::vector<unsigned char> createEmptyImage(const TargetImageParameters &image_params);
} // nameapce image::helper