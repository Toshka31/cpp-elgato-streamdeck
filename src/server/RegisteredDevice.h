#pragma once

#include "ConfigLoader.h"

#include "Profile.h"
#include "ModuleLoader.h"

#include <StreamDeckLib/ImageHelper/ImageHelper.h>
#include <StreamDeckLib/Device/IStreamDeck.h>

#include <memory>
#include <functional>

class RegisteredDevice : public std::enable_shared_from_this<RegisteredDevice>
{
public:
    RegisteredDevice(std::shared_ptr<IStreamDeck> deck, std::shared_ptr<ModuleLoader> module_loader);

    void tick();

    bool is_device_open();

    void setBrightness(ushort brightness);

    void setButtonImage(ushort key, std::vector<uint8_t> &image);

    void setButtonLabel(ushort key, const std::string &label);

    void setButtonComponent(ushort key, const std::string &module, const std::string &component);

    std::string getCurrentProfileName() const;

    void setProfile(const std::string &profile_name);

    std::vector<std::string> getProfiles();

    std::string getCurrentPageName() const;

    std::vector<std::string> getPages();

    image::helper::TargetImageParameters getImageFormat();

private:
    void callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val);

    void updateButton(ushort key);

    void addProfileFromModule(const std::string &module, const ProvidedProfile &profile);

protected:
    image::helper::TargetImageParameters m_image_params;
    std::shared_ptr<IStreamDeck> m_streamdeck;
    std::shared_ptr<ModuleLoader> m_module_loader;
    Profile m_current_profile;
    std::map<ushort, std::shared_ptr<IComponent>> m_key_mapping;

    struct Event
    {
        Event(ushort key, bool val) : key(key), value(val) {}
        
        ushort key;
        bool value;
    };
    std::mutex m_mutex;
    std::vector<Event> m_events;
};
