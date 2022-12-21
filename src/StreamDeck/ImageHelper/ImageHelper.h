#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/dynamic_step.hpp>


#include <fstream>
#include <iterator>
#include <sstream>
#include <ios>

namespace image::helper {

void resizeImage(boost::gil::rgb8_image_t &img)
{
    boost::gil::rgb8_image_t sized(72, 72);
    boost::gil::resize_view(boost::gil::view(img), boost::gil::view(sized), boost::gil::bilinear_sampler{});
    img = sized;
}

void flipImage(boost::gil::rgb8_image_t &img, bool vertically, bool horizontally)
{
    boost::gil::flipped_left_right_view(boost::gil::const_view(img));
    boost::gil::flipped_up_down_view(boost::gil::const_view(img));
}

std::vector<unsigned char> loadFromFile(const std::string &filename)
{
    boost::gil::rgb8_image_t img;
    boost::gil::read_image(filename, img, boost::gil::jpeg_tag{});

    if (img.width() != 72 || img.height() != 72) // TODO replace magic numbers with numbers from device parameters
        resizeImage(img);

    flipImage(img, true, true); // 

    std::stringstream out_buffer(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    boost::gil::write_view( out_buffer, boost::gil::view(img), boost::gil::jpeg_tag() );

    std::streampos size;
    out_buffer.seekg(0, std::ios::end);
    size = out_buffer.tellg();
    out_buffer.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec;
    vec.reserve(size);

    while (!out_buffer.eof())
        vec.push_back(out_buffer.get());

    //vec.insert(vec.begin(),
    //        std::istream_iterator<unsigned char>(out_buffer),
    //        std::istream_iterator<unsigned char>());

    auto test = vec.size();

    return vec;
}

} // nameapce image::helper