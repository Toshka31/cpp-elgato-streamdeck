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

        m_image_params = {
                m_streamdeck->key_image_format().size.first,
                m_streamdeck->key_image_format().size.second,
                m_streamdeck->key_image_format().flip.first,
                m_streamdeck->key_image_format().flip.second };

        // apply profile
        unsigned short key_count = m_streamdeck->key_count();
        for (auto key = 0; key < key_count; ++key)
        {
            auto key_profile = m_current_profile.getCurrentKeyProfile(key);
            if (!key_profile.m_module_name.empty() && !key_profile.m_component_name.empty())
                setButtonComponent(key, key_profile.m_module_name, key_profile.m_component_name);

            std::vector<unsigned char> image_data;
            if (!key_profile.m_custom_image.empty())
                image_data = image::helper::loadRawImage(key_profile.m_custom_image);

            if (!key_profile.m_custom_label.empty()) {
                if (image_data.empty())
                    image_data = image::helper::createEmptyImage(m_image_params);
                image_data = image::helper::applyLabelOnImage(image_data, key_profile.m_custom_label);
            }

            if (!image_data.empty()) {
                image_data = image::helper::prepareImageForDeck(image_data, m_image_params);
                m_streamdeck->set_key_image(key, image_data);
            }
        }
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

    void setButtonImage(ushort key, std::vector<uint8_t> &image)
    {
        // cache image
        auto saved_path = saveButtonImageForDeck(m_streamdeck->get_serial_number(), image);

        auto prepared_image = image::helper::prepareImageForDeck(image, m_image_params);

        m_streamdeck->set_key_image(key, prepared_image);

        // save to profile
        m_current_profile.setButtonImage(key, saved_path);
    }

    void setButtonLabel(ushort key, const std::string &label)
    {
        auto key_profile = m_current_profile.getCurrentKeyProfile(key);
        std::vector<uint8_t> image_data;
        if (!key_profile.m_custom_image.empty())
            image_data = image::helper::loadRawImage(key_profile.m_custom_image);
        else
            image_data = image::helper::createEmptyImage(m_image_params);

        image_data = image::helper::applyLabelOnImage(image_data, label);

        image_data = image::helper::prepareImageForDeck(image_data, m_image_params);
        m_streamdeck->set_key_image(key, image_data);

        // save to profile
        m_current_profile.setButtonLabel(key, label);
    }

    void setButtonComponent(ushort key, const std::string &module, const std::string &component)
    {
        auto comp = m_module_loader->getModuleComponent(module, component);
        auto image = comp->getImage();
        m_streamdeck->set_key_image(key, image::helper::prepareImageForDeck(image, m_image_params));
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
