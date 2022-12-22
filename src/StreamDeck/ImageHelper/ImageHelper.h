#include <vector>
#include <string>

namespace image::helper {

struct TargetImageParameters 
{
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

std::vector<unsigned char> prepareImageForDeck(std::vector<unsigned char> &image_file_raw_data, EImageFormat format, const TargetImageParameters &image_params);

std::vector<unsigned char> prepareImageForDeck(const std::string &filename, const TargetImageParameters &image_params);

} // nameapce image::helper