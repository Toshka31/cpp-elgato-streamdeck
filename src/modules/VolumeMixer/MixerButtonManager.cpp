#include "images/images.h"

#include "MixerButtonManager.h"

std::shared_ptr<MixerButtonManager> MixerButtonManager::getManager(const std::string &streamdeck_id)
{
    const auto it = m_map_manager.find(streamdeck_id);
    if (it != m_map_manager.end())
        return it->second;
    else
        return m_map_manager.insert({streamdeck_id, std::make_shared<MixerButtonManager>()}).first->second;
}

void MixerButtonManager::tickButton(unsigned short key)
{
    if (key == 0)
        update();
    else if ( key >= 6 && key <= 9)
    {
        // TODO
    }
}

void MixerButtonManager::pressButton(unsigned short key)
{
    if (key == 0)
    {
        // TODO go to previous profile
    }
    else if ( key >= 1 && key <= 4)
    {
        // TODO volume up
    }
    else if ( key >= 11 && key <= 14)
    {
        // TODO volume down
    }
    else if ( key == 5)
    {
        // TODO go to next page
    }
    else if ( key == 10)
    {
        // TODO go to previous page
    }
}

bool MixerButtonManager::needImageUpdate(unsigned short key)
{
    if ( key >= 6 && key <= 9)
        return true;
    else
        return false;
}

std::vector<unsigned char> MixerButtonManager::getButtonImage(unsigned short key)
{
    if (key == 0)
        return IMAGE_HOME;
    else if (key == 5)
        return IMAGE_FORWARD;
    else if (key == 10)
        return IMAGE_BACK;
    else if ( key >= 1 && key <= 4)
        return IMAGE_PLUS;
    else if ( key >= 11 && key <= 14)
        return IMAGE_MINUS;
    else if ( key >= 6 && key <= 9) {
        if (m_key_apps.find(key - 5) != m_key_apps.end())
            return PulseSystem::getIcon(m_key_apps[key - 5]);
        else
            return {};
    }
    return {};
}

void MixerButtonManager::update()
{
    m_key_apps.clear();

    const auto volumes = m_pulse_system.getVolumes();
    unsigned short col = 1;
    for (const auto &vol : volumes)
    {
        m_key_apps[col] = vol.first;
        col++;
    }
}

std::map<std::string, std::shared_ptr<MixerButtonManager>> MixerButtonManager::m_map_manager;