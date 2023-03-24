#pragma once

#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <string>
#include <vector>

class IDeviceButtonRestricted
{
public:
    virtual void setBrightness(unsigned short brightness) = 0;

    virtual void setProfile(const std::string &profile_name) = 0;

    virtual void setButtonImage(std::vector<uint8_t> &image) = 0;

    virtual image::helper::TargetImageParameters getImageFormat() = 0;
};