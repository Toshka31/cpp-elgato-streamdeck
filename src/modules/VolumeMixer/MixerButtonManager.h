#pragma once

#include "PulseSystem.h"

#include <memory>

class MixerButtonManager
{
public:
    static std::shared_ptr<MixerButtonManager> getManager(const std::string &streamdeck_id);

    MixerButtonManager() = default;
    ~MixerButtonManager() = default;

    void tickButton(unsigned short key);

    void pressButton(unsigned short key);

    bool needImageUpdate(unsigned short key);

    std::vector<unsigned char> getButtonImage(unsigned short key);

private:
    void update();

private:
    PulseSystem m_pulse_system;

    std::map<unsigned short, std::string> m_key_apps;

    static std::map<std::string, std::shared_ptr<MixerButtonManager>> m_map_manager;
};
