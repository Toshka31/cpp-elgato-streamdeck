#include "MixerComponent.h"

void MixerComponent::init(std::shared_ptr<IDeviceButtonRestricted> device)
{
    m_device = device;
    m_manager = MixerButtonManager::getManager(m_device->getID());
}

std::string MixerComponent::name() const
{
    return "MixerComponent";
}

void MixerComponent::tick()
{
    m_manager->tickButton(m_device->getOwnedKey());
    m_device->updateButtonImage();
}

void MixerComponent::actionPress()
{
    m_manager->pressButton(m_device->getOwnedKey());
}

void MixerComponent::actionRelease()
{
// pass
}

std::vector<unsigned char> MixerComponent::getImage() const
{
    return m_manager->getButtonImage(m_device->getOwnedKey());
}
