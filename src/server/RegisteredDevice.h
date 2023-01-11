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

private:
    void setKeyComponent(std::shared_ptr<IStreamDeck> deck, ushort key, std::shared_ptr<IComponent> component)
    {
        image::helper::TargetImageParameters image_params = { 
            deck->key_image_format().size.first, 
            deck->key_image_format().size.second, 
            deck->key_image_format().flip.first, 
            deck->key_image_format().flip.second };
        image::helper::EImageFormat format;
        auto image = component->getImage(format);
        deck->set_key_image(key, image::helper::prepareImageForDeck(image, format, image_params));
        m_key_mapping[key] = component;
    }

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