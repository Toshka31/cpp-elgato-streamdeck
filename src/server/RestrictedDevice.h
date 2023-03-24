#pragma once

#include <ModuleAPI/IDeviceButtonRestricted.h>

#include <memory>

class RegisteredDevice;

class RestrictedDevice : public IDeviceButtonRestricted
{
public:
    RestrictedDevice(unsigned short key, std::shared_ptr<RegisteredDevice> device);

    void setBrightness(unsigned short brightness) override;

    void setProfile(const std::string &profile_name) override;

    void setButtonImage(std::vector<uint8_t> &image) override;

    image::helper::TargetImageParameters getImageFormat() override;
private:
    unsigned short m_key;
    std::shared_ptr<RegisteredDevice> m_device;
};
