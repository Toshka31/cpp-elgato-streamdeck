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

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace {
    void resizeImage(cv::Mat &image)
    {
        cv::Mat out(cv::Size(72, 72), CV_8UC1);
        cv::resize(image, out, out.size());
        image = out;
    }

    void flipImage(cv::Mat &image, bool vertically, bool horizontally)
    {
        cv::Mat out = image;
        if (vertically && horizontally)
            cv::flip(image, out, -1);
        else if (vertically)
            cv::flip(image, out, 0);
        else if (horizontally)
            cv::flip(image, out, 1);
        image = out;
    }
}
namespace image::helper {

    TargetImageParameters::TargetImageParameters(unsigned short _width,
                                                 unsigned short _height,
                                                 bool _flip_verticaly,
                                                 bool _flip_horizontaly)
            : width(_width),
              height(_height),
              flip_verticaly(_flip_verticaly),
              flip_horizontaly(_flip_horizontaly)
    { }

    std::vector<unsigned char> prepareImageForDeck(cv::Mat &image,
                                                   const image::helper::TargetImageParameters &image_params)
    {
        if (image.size().width != image_params.width || image.size().height != image_params.height)
            resizeImage(image);

        flipImage(image, image_params.flip_verticaly, image_params.flip_horizontaly);

        std::vector<unsigned char> out;
        cv::imencode(".jpg", image, out);

        return out;
    }

    std::vector<unsigned char> prepareImageForDeck(const std::string &filename,
                                                   const image::helper::TargetImageParameters &image_params)
    {
        cv::Mat image = cv::imread(filename, cv::IMREAD_COLOR);

        return prepareImageForDeck(image, image_params);
    }

    std::vector<unsigned char> prepareImageForDeck(std::vector<unsigned char> &image_file_raw_data,
                                                   const ::image::helper::TargetImageParameters &image_params)
    {
        cv::Mat image = cv::imdecode(cv::Mat(image_file_raw_data), cv::IMREAD_COLOR);

        return prepareImageForDeck(image, image_params);
    }

    void applyLabelOnImage(std::vector<unsigned char> &image_file_raw_data, const std::string &label)
    {
        cv::Mat image = cv::imdecode(cv::Mat(image_file_raw_data), cv::IMREAD_COLOR);

        cv::putText(image, label,
                    cv::Point(10, image.rows / 2), //top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(118, 185, 0), //font color
                    2);

        image_file_raw_data.clear();
        cv::imencode(".jpg", image, image_file_raw_data);
    }
}