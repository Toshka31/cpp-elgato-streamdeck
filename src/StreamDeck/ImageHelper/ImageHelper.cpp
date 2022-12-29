#include <StreamDeckLib/ImageHelper/ImageHelper.h>

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

#include <typeinfo>
#include <iostream> 

namespace {

void resizeImage(boost::gil::rgb8_image_t &img, size_t width = 72, size_t height = 72)
{
    boost::gil::rgb8_image_t sized(width, height);
    boost::gil::resize_view(boost::gil::view(img), boost::gil::view(sized), boost::gil::bilinear_sampler{});
    img = sized;
}

auto flipImage(boost::gil::rgb8_image_t &img, bool vertically, bool horizontally)
{
    // did not find the proper way to create dynamic view, so rotate
    auto view = boost::gil::rotated90ccw_view(boost::gil::rotated90cw_view(boost::gil::const_view(img)));
    if (vertically)
        view = boost::gil::flipped_left_right_view(view);
    if (horizontally)
        view =  boost::gil::flipped_up_down_view(view);
    return view;
}

} // unnamed namespace

namespace image::helper {

TargetImageParameters::TargetImageParameters(
        unsigned short _width,
        unsigned short _height,
        bool _flip_verticaly,
        bool _flip_horizontaly
) : width(_width),
    height(_height),
    flip_verticaly(_flip_verticaly),
    flip_horizontaly(_flip_horizontaly)
{}

std::vector<unsigned char> prepareImageForDeck(boost::gil::rgb8_image_t &img, const TargetImageParameters &image_params)
{
    if (img.width() != image_params.width || img.height() != image_params.height)
        resizeImage(img);

    auto flippedView = flipImage(img, image_params.flip_verticaly, image_params.flip_horizontaly);
    std::stringstream out_buffer(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    boost::gil::write_view(out_buffer, flippedView, boost::gil::jpeg_tag{});

    out_buffer.seekg(0, std::ios::end);
    const std::streampos size = out_buffer.tellg();
    out_buffer.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec;
    vec.reserve(size);

    while (!out_buffer.eof())
        vec.push_back(out_buffer.get());

    return vec;
}

std::vector<unsigned char> prepareImageForDeck(
        std::vector<unsigned char> &image_file_raw_data,
        EImageFormat format,
        const TargetImageParameters &image_params)
{
    // TODO: think about possible implementation to support raw RGB 8-bit
    boost::gil::rgb8_image_t img;

    std::stringstream in_buffer( std::ios_base::in | std::ios_base::out | std::ios_base::binary );
    std::cout << image_file_raw_data.size() << std::endl;
    for (auto c : image_file_raw_data)
        in_buffer << c;
    //std::copy(image_file_raw_data.begin(), image_file_raw_data.end(), std::ostream_iterator<unsigned char>(in_buffer));
    
    switch (format)
    {
    case EImageFormat::JPEG:
        boost::gil::read_image(in_buffer, img, boost::gil::jpeg_tag{});
        break;
    case EImageFormat::PNG:
        boost::gil::read_image(in_buffer, img, boost::gil::png_tag{});
        break;
    default:
        throw std::runtime_error("Format not supported");
        break;
    }

    return prepareImageForDeck(img, image_params);
}

std::vector<unsigned char> prepareImageForDeck(const std::string &filename, const TargetImageParameters &image_params)
{
    boost::gil::rgb8_image_t img;

    // deduce format by extension
    auto point_pos = filename.rfind(".") + 1;
    std::string ext_str = filename.substr(point_pos);
    if (ext_str == "jpg")
        boost::gil::read_image(filename, img, boost::gil::jpeg_tag{});
    else if (ext_str == "png")
    {
        boost::gil::rgba8_image_t png_img;
        boost::gil::read_image(filename, png_img, boost::gil::png_tag{});
        img = boost::gil::rgb8_image_t(png_img.width(),png_img.height());
        boost::gil::copy_and_convert_pixels(
            boost::gil::const_view(png_img),
            boost::gil::view(img)
        );
    }

    return prepareImageForDeck(img, image_params);
}

} // nameapce image::helper
