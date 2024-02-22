#pragma once

#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <string>
#include <vector>

class IDeviceButtonRestricted
{
public:
    virtual std::string getID() = 0;

    virtual unsigned short getOwnedKey() = 0;

    virtual void setBrightness(unsigned short brightness) = 0;

    virtual std::string getCurrentProfileName() = 0;

    virtual void setProfile(const std::string &profile_name) = 0;

    virtual void updateButtonImage() = 0;

    virtual image::helper::TargetImageParameters getImageFormat() = 0;
};