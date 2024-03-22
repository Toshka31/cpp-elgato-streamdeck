#pragma once

#include "module.h"
#include "MixerButtonManager.h"

class MixerComponent : public IComponent {
    DECLARE_MODULE_COMPONENT(MixerModule, MixerComponent)
    COMPONENT_SETTINGS_EMPTY
public:
    void init(std::shared_ptr<IDeviceButtonRestricted> device) override;

    std::string name() const override;

    void tick() override;

    void actionPress() override;

    void actionRelease() override;

    std::vector<unsigned char> getImage() const override;
private:
    std::shared_ptr<IDeviceButtonRestricted> m_device;
    std::shared_ptr<MixerButtonManager> m_manager;
};