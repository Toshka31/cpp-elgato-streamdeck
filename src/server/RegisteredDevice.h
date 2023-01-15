#pragma once

#include "Profile.h"
#include "ModuleLoader.h"

#include <StreamDeckLib/ImageHelper/ImageHelper.h>
#include <StreamDeckLib/Device/IStreamDeck.h>

#include <memory>
#include <functional>

class RegisteredDevice
{
public:
    RegisteredDevice(std::shared_ptr<IStreamDeck> deck, Profile profile)
        : m_streamdeck(deck), m_current_profile(profile) {}

    void init(std::shared_ptr<ModuleLoader> mod_loader)
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

        for (auto key : m_key_mapping)
            key.second->tick();
    }

    bool is_device_open()
    {
        return m_streamdeck->is_open();
    }

    void setBrightness(ushort brightness)
    {
        // TODO save to profile
        m_streamdeck->set_brightness(brightness);
    }

    void setButtonImage(ushort key, std::vector<uint8_t> image, image::helper::EImageFormat format)
    {
        // TODO save to profile
        image::helper::TargetImageParameters image_params = { 
            m_streamdeck->key_image_format().size.first, 
            m_streamdeck->key_image_format().size.second, 
            m_streamdeck->key_image_format().flip.first, 
            m_streamdeck->key_image_format().flip.second };
        std::cout << "image.size() = " << image.size() << std::endl;
        m_streamdeck->set_key_image(key, image::helper::prepareImageForDeck(image, format, image_params));
    }

    void setButtonComponent(ushort key, std::shared_ptr<IComponent> component)
    {
        // TODO save to profile
        image::helper::TargetImageParameters image_params = { 
            m_streamdeck->key_image_format().size.first,
            m_streamdeck->key_image_format().size.second,
            m_streamdeck->key_image_format().flip.first,
            m_streamdeck->key_image_format().flip.second };
        image::helper::EImageFormat format;
        auto image = component->getImage(format);
        m_streamdeck->set_key_image(key, image::helper::prepareImageForDeck(image, format, image_params));
        m_key_mapping[key] = component;
    }

private:
    void callback(std::shared_ptr<IStreamDeck> deck, ushort key, bool val)
    {
        std::lock_guard<std::mutex>  lock(m_mutex);
        m_events.emplace_back(key, val);
    }

protected:
    std::shared_ptr<IStreamDeck> m_streamdeck;
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