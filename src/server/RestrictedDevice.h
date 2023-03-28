#pragma once

#include <ModuleAPI/IDeviceButtonRestricted.h>

#include <memory>

class RegisteredDevice;

class RestrictedDevice : public IDeviceButtonRestricted
{
public:
    RestrictedDevice(unsigned short key, std::shared_ptr<RegisteredDevice> device);

    std::string getID() override;

    unsigned short getOwnedKey() override;

    void setBrightness(unsigned short brightness) override;

    void setProfile(const std::string &profile_name) override;

    void updateButtonImage() override;

    image::helper::TargetImageParameters getImageFormat() override;
private:
    unsigned short m_key;
    std::shared_ptr<RegisteredDevice> m_device;
};
