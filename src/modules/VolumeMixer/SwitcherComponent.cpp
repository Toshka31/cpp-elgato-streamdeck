
#include "module.h"
#include "SwitcherComponent.h"

void MixerSwitchComponent::init(std::shared_ptr<IDeviceButtonRestricted> device)
{
    m_device = device;
}

std::string MixerSwitchComponent::name() const
{
    return "Show Mixer";
}

void MixerSwitchComponent::tick()
{

}

void MixerSwitchComponent::actionPress()
{
    m_device->setProfile("MixerModule");
}

void MixerSwitchComponent::actionRelease()
{

}

std::vector<unsigned char> MixerSwitchComponent::getImage() const
{
    return {};
}