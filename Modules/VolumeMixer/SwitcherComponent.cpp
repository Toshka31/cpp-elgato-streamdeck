
#include "module.h"
#include "SwitcherComponent.h"
#include "MixerButtonManager.h"

#include "images/mixer.h"

void MixerSwitchComponent::init(std::shared_ptr<IDeviceButtonRestricted> device) {
    m_device = device;
}

std::string MixerSwitchComponent::name() const {
    return "Show Mixer";
}

void MixerSwitchComponent::tick() {

}

void MixerSwitchComponent::actionPress() {
    std::string cur_profile_name = m_device->getCurrentProfileName();
    MixerButtonManager::getManager(m_device)->setPreviousProfileName(cur_profile_name);
    m_device->setProfile("MixerModule");
}

void MixerSwitchComponent::actionRelease() {

}

std::vector<unsigned char> MixerSwitchComponent::getImage() const {
    return IMAGE_MIXER;
}