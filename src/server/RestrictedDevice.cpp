#include "RestrictedDevice.h"
#include "RegisteredDevice.h"

RestrictedDevice::RestrictedDevice(unsigned short key, std::shared_ptr<RegisteredDevice> device)
    : m_key(key), m_device(device)
{

}

std::string RestrictedDevice::getID()
{
    return m_device->getID();
}

unsigned short RestrictedDevice::getOwnedKey()
{
    return m_key;
}

void RestrictedDevice::setBrightness(unsigned short brightness)
{
    m_device->setBrightness(brightness);
}

std::string RestrictedDevice::getCurrentProfileName()
{
    return m_device->getCurrentProfileName();
}

void RestrictedDevice::setProfile(const std::string &profile_name)
{
    m_device->setProfile(profile_name);
}

void RestrictedDevice::updateButtonImage()
{
    m_device->updateButtonImage(m_key);
}

image::helper::TargetImageParameters RestrictedDevice::getImageFormat()
{
    return m_device->getImageFormat();
}
