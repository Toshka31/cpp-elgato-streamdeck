#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/dynamic_step.hpp>
#include <boost/gil/io/io.hpp>


#include <fstream>
#include <iterator>
#include <sstream>
#include <ios>

namespace image::helper {

namespace {

void resizeImage(boost::gil::rgb8_image_t &img)
{
    boost::gil::rgb8_image_t sized(72, 72);
    boost::gil::resize_view(boost::gil::view(img), boost::gil::view(sized), boost::gil::bilinear_sampler{});
    img = sized;
}

auto flipImage(boost::gil::rgb8_image_t &img, bool vertically, bool horizontally)
{
    return boost::gil::flipped_left_right_view(boost::gil::flipped_up_down_view(boost::gil::const_view(img)));
}

} // unnamed namespace

std::vector<unsigned char> prepareImageForDeck(std::vector<unsigned char> &image_file_raw_data, unsigned short width, unsigned short height, bool flip_verticaly, bool flip_horizontaly)
{
    // TODO: think about possible implementation
    // raw data can be either png file data or jpeg
    // also we can support raw RGB 8-bit
}

std::vector<unsigned char> prepareImageForDeck(const std::string &filename, unsigned short width, unsigned short height, bool flip_verticaly, bool flip_horizontaly)
{
    boost::gil::rgb8_image_t img;

    // deduce format by extension
    auto point_pos = filename.rfind(".") + 1;
    std::string ext_str = filename.substr(point_pos);
    if(ext_str== "jpg")
        boost::gil::read_image(filename, img, boost::gil::jpeg_tag{});
    else if (ext_str== "png")
    {
        boost::gil::rgba8_image_t png_img;
        boost::gil::read_image(filename, png_img, boost::gil::png_tag{});
        img = boost::gil::rgb8_image_t(png_img.width(),png_img.height());
        boost::gil::copy_and_convert_pixels(
            boost::gil::const_view(png_img),
            boost::gil::view(img)
        );
    }

    if (img.width() != width || img.height() != height)
        resizeImage(img);

    auto flippedView = flipImage(img, flip_verticaly, flip_horizontaly);

    std::stringstream out_buffer(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    boost::gil::write_view( out_buffer, flippedView, boost::gil::jpeg_tag() );

    std::streampos size;
    out_buffer.seekg(0, std::ios::end);
    size = out_buffer.tellg();
    out_buffer.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec;
    vec.reserve(size);

    while (!out_buffer.eof())
        vec.push_back(out_buffer.get());

    auto test = vec.size();

    return vec;
}

} // nameapce image::helper