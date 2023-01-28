#pragma once

#include "ConfigLoader.h"
#include "Profile.h"
#include "ModuleLoader.h"

#include <StreamDeckLib/ImageHelper/ImageHelper.h>
#include <StreamDeckLib/Device/IStreamDeck.h>

#include <memory>
#include <functional>

class RegisteredDevice
{
public:
    RegisteredDevice(std::shared_ptr<IStreamDeck> deck, std::shared_ptr<ModuleLoader> module_loader)
        : m_streamdeck(deck),
          m_current_profile(loadDeckProfile(deck->get_serial_number())),
          m_module_loader(module_loader)
    {
        m_streamdeck->set_key_callback(std::bind(&RegisteredDevice::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void tick()
    {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            for (auto event : m_events)
            {
                std::cout << "["<< m_streamdeck->get_serial_number() << "]:" << "\t" << event.key << "\t" << event.value << std::endl;

                auto it = m_key_mapping.find(event.key);
                if (it != m_key_mapping.end())
                {
                    if (event.value)
                        it->second->actionPress(m_streamdeck, event.key);
                    else
                        it->second->actionRelease(m_streamdeck, event.key);
                }
            }
            m_events.clear();
        }

        for (auto &key : m_key_mapping)
            key.second->tick();
    }

    bool is_device_open()
    {
        return m_streamdeck->is_open();
    }

    void setBrightness(ushort brightness)
    {
        m_current_profile.setBrightness(brightness);
        m_streamdeck->set_brightness(brightness);
    }

    void setButtonImage(ushort key, std::vector<uint8_t> image, image::helper::EImageFormat format)
    {
        image::helper::TargetImageParameters image_params = { 
            m_streamdeck->key_image_format().size.first, 
            m_streamdeck->key_image_format().size.second, 
            m_streamdeck->key_image_format().flip.first, 
            m_streamdeck->key_image_format().flip.second };
        auto prepared_image = image::helper::prepareImageForDeck(image, format, image_params);

        // cache image
        auto saved_path = saveButtonImageForDeck(m_streamdeck->get_serial_number(), prepared_image);

        m_streamdeck->set_key_image(key, prepared_image);

        // save to profile
        m_current_profile.setButtonImage(key, saved_path);
    }

    void setButtonComponent(ushort key, const std::string &module, const std::string &component)
    {
        auto comp = m_module_loader->getModuleComponent(module, component);
        image::helper::TargetImageParameters image_params = { 
            m_streamdeck->key_image_format().size.first,
            m_streamdeck->key_image_format().size.second,
            m_streamdeck->key_image_format().flip.first,
            m_streamdeck->key_image_format().flip.second };
        image::helper::EImageFormat format;
        auto image = comp->getImage(format);
        m_streamdeck->set_key_image(key, image::helper::prepareImageForDeck(image, format, image_params));
        m_key_mapping[key] = comp;

        // save to profile
        m_current_profile.setButtonComponent(key, module, component);
    }

    std::string getCurrentProfileName() const
    {
        return m_current_profile.getName();
    }

    std::vector<std::string> getProfiles()
    {
        return getDeckProfiles(m_streamdeck->get_serial_number());
    }

    std::string getCurrentPageName() const
    {
        return m_current_profile.getCurrentPageName();
    }

    std::vector<std::string> getPages()
    {
        return m_current_profile.getPages();
    }

private:
    void callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_events.emplace_back(key, val);
    }

protected:
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
