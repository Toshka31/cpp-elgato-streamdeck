#include "RestrictedDevice.h"
#include "RegisteredDevice.h"

RestrictedDevice::RestrictedDevice(unsigned short key, std::shared_ptr<RegisteredDevice> device)
    : m_key(key), m_device(device)
{

}

void RestrictedDevice::setBrightness(unsigned short brightness)
{
    m_device->setBrightness(brightness);
}

void RestrictedDevice::setProfile(const std::string &profile_name)
{
    m_device->setProfile(profile_name);
}

void RestrictedDevice::setButtonImage(std::vector<uint8_t> &image)
{
    m_device->setButtonImage(m_key, image);
}

image::helper::TargetImageParameters RestrictedDevice::getImageFormat()
{
    return m_device->getImageFormat();
}
