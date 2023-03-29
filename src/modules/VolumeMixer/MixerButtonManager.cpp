#include "images/images.h"

#include "MixerButtonManager.h"

constexpr unsigned short BUTTONS_IN_ROW = 5;
constexpr unsigned short BUTTONS_APPS_IN_ROW = BUTTONS_IN_ROW - 1;

constexpr unsigned short BUTTON_HOME_INDEX = 0;
constexpr unsigned short BUTTON_FORWARD_INDEX = BUTTONS_IN_ROW * 1; // first in second row
constexpr unsigned short BUTTON_BACK_INDEX = BUTTONS_IN_ROW * 2; // first in third row

constexpr unsigned short BUTTON_VOLUME_UP_FIRST_COLUMN_INDEX = 1;
constexpr unsigned short BUTTON_VOLUME_UP_LAST_COLUMN_INDEX = BUTTON_VOLUME_UP_FIRST_COLUMN_INDEX + BUTTONS_APPS_IN_ROW - 1;

constexpr unsigned short BUTTON_VOLUME_DOWN_FIRST_COLUMN_INDEX = BUTTONS_IN_ROW * 2 + 1;
constexpr unsigned short BUTTON_VOLUME_DOWN_LAST_COLUMN_INDEX = BUTTON_VOLUME_DOWN_FIRST_COLUMN_INDEX + BUTTONS_APPS_IN_ROW - 1;

constexpr unsigned short BUTTON_APP_FIRST_COLUMN_INDEX = BUTTONS_IN_ROW * 1 + 1;
constexpr unsigned short BUTTON_APP_LAST_COLUMN_INDEX = BUTTON_APP_FIRST_COLUMN_INDEX + BUTTONS_APPS_IN_ROW - 1;


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
    if (key == BUTTON_HOME_INDEX)
        update();
}

void MixerButtonManager::pressButton(unsigned short key)
{
    if (key == BUTTON_HOME_INDEX)
    {
        m_device->setProfile(m_previous_profile_name);
    }
    else if ( key >= BUTTON_VOLUME_UP_FIRST_COLUMN_INDEX && key <= BUTTON_VOLUME_UP_LAST_COLUMN_INDEX)
    {
        const auto it = m_key_apps.find(key % BUTTONS_IN_ROW);
        if (it != m_key_apps.end())
            m_pulse_system.volumeUp(it->second.name);
    }
    else if ( key >= BUTTON_VOLUME_DOWN_FIRST_COLUMN_INDEX && key <= BUTTON_VOLUME_DOWN_LAST_COLUMN_INDEX)
    {
        const auto it = m_key_apps.find(key % BUTTONS_IN_ROW);
        if (it != m_key_apps.end())
            m_pulse_system.volumeDown(it->second.name);
    }
    else if ( key == BUTTON_FORWARD_INDEX && m_key_apps.size() > BUTTONS_APPS_IN_ROW * m_current_page)
        m_current_page += 1;
    else if ( key == BUTTON_BACK_INDEX && m_current_page != 1)
        m_current_page -= 1;
}

std::vector<unsigned char> MixerButtonManager::getButtonImage(unsigned short key)
{
    if (key == BUTTON_HOME_INDEX)
        return IMAGE_HOME;
    else if (key == BUTTON_FORWARD_INDEX && m_key_apps.size() > BUTTONS_APPS_IN_ROW * m_current_page)
        return IMAGE_FORWARD;
    else if (key == BUTTON_BACK_INDEX && m_current_page != 1)
        return IMAGE_BACK;
    else if ( key >= BUTTON_VOLUME_UP_FIRST_COLUMN_INDEX && key <= BUTTON_VOLUME_UP_LAST_COLUMN_INDEX && m_key_apps.find(key) != m_key_apps.end())
        return IMAGE_PLUS;
    else if ( key >= BUTTON_VOLUME_DOWN_FIRST_COLUMN_INDEX && key <= BUTTON_VOLUME_DOWN_LAST_COLUMN_INDEX && m_key_apps.find(key % BUTTONS_IN_ROW) != m_key_apps.end())
        return IMAGE_MINUS;
    else if ( key >= BUTTON_APP_FIRST_COLUMN_INDEX && key <= BUTTON_APP_LAST_COLUMN_INDEX && m_key_apps.find(key % BUTTONS_IN_ROW) != m_key_apps.end())
        return PulseSystem::getIcon(m_key_apps[key % BUTTONS_IN_ROW].icon);
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