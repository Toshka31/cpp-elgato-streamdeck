#pragma once

#include "ModuleAPI/IDeviceButtonRestricted.h"

#include "PulseSystem.h"

#include <memory>

class MixerButtonManager {
public:
    static std::shared_ptr<MixerButtonManager> getManager(const std::shared_ptr<IDeviceButtonRestricted>& device);

    explicit MixerButtonManager(std::shared_ptr<IDeviceButtonRestricted>);
    ~MixerButtonManager() = default;

    void setPreviousProfileName(const std::string &profile_name);

    void tickButton(unsigned short key);

    void pressButton(unsigned short key);

    std::vector<unsigned char> getButtonImage(unsigned short key);

private:
    void update();

private:
    std::shared_ptr<IDeviceButtonRestricted> m_device;
    std::string m_previous_profile_name;

    PulseSystem m_pulse_system;

    unsigned short m_current_page = 1;
    std::map<unsigned short, PulseSystem::AppVolume> m_key_apps;

    static std::map<std::string, std::shared_ptr<MixerButtonManager>> m_map_manager;
};
