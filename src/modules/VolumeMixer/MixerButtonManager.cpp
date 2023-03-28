#include "images/images.h"

#include "MixerButtonManager.h"

std::shared_ptr<MixerButtonManager> MixerButtonManager::getManager(std::shared_ptr<IDeviceButtonRestricted> device)
{
    const auto it = m_map_manager.find(device->getID());
    if (it != m_map_manager.end())
        return it->second;
    else
        return m_map_manager.insert({device->getID(), std::make_shared<MixerButtonManager>(device)}).first->second;
}

MixerButtonManager::MixerButtonManager(std::shared_ptr<IDeviceButtonRestricted> device)
    : m_device(device) { }

void MixerButtonManager::setPreviousProfileName(const std::string &profile_name)
{
    m_previous_profile_name = profile_name;
}

void MixerButtonManager::tickButton(unsigned short key)
{
    if (key == 0)
        update();
    else if ( key >= 6 && key <= 9)
    {
        // TODO display volumes as label on image
    }
}

void MixerButtonManager::pressButton(unsigned short key)
{
    if (key == 0)
    {
        m_device->setProfile(m_previous_profile_name);
    }
    else if ( key >= 1 && key <= 4)
    {
        const auto it = m_key_apps.find(key % 5);
        if (it != m_key_apps.end())
            m_pulse_system.volumeUp(it->second.name);
    }
    else if ( key >= 11 && key <= 14)
    {
        const auto it = m_key_apps.find(key % 5);
        if (it != m_key_apps.end())
            m_pulse_system.volumeDown(it->second.name);
    }
    else if ( key == 5 && m_key_apps.size() > 4 * m_current_page)
        m_current_page += 1;
    else if ( key == 10 && m_current_page != 1)
        m_current_page -= 1;
}

std::vector<unsigned char> MixerButtonManager::getButtonImage(unsigned short key)
{
    if (key == 0)
        return IMAGE_HOME;
    else if (key == 5 && m_key_apps.size() > 4 * m_current_page)
        return IMAGE_FORWARD;
    else if (key == 10 && m_current_page != 1)
        return IMAGE_BACK;
    else if ( key >= 1 && key <= 4 && m_key_apps.find(key) != m_key_apps.end())
        return IMAGE_PLUS;
    else if ( key >= 11 && key <= 14 && m_key_apps.find(key % 5) != m_key_apps.end())
        return IMAGE_MINUS;
    else if ( key >= 6 && key <= 9 && m_key_apps.find(key - 5) != m_key_apps.end())
        return PulseSystem::getIcon(m_key_apps[key % 5].icon);
    return {};
}

void MixerButtonManager::update()
{
    m_key_apps.clear();

    auto volumes = m_pulse_system.getVolumes();
    unsigned short col = 1;
    for (auto &vol : volumes)
    {
        m_key_apps[col] = vol;
        col++;
    }
}

std::map<std::string, std::shared_ptr<MixerButtonManager>> MixerButtonManager::m_map_manager;