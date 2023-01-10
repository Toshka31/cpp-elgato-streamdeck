#include "ModuleLoader.h"

#include <StreamDeckLib/DeviceManager.h>
#include <StreamDeckLib/Transport/TransportFactory.h>
#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <chrono>
#include <thread>
#include <mutex>

using namespace std::chrono_literals;

class Profile
{
public:
    void load()
    {

    }
    void save()
    {

    }

    struct Key
    {

    };

    std::map<short, Key> m_keys;
};

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


Profile loadProfile(const std::string &deck_serial)
{
    // TODO load from file
    return Profile();
}

class Engine
{
public:
    Engine()
    {
        m_module_loader = std::make_shared<ModuleLoader>("/home/anton/dev/streamdeck/build/src/modules/ALSA");

        DeviceManager mngr(TransportFactory::createUsbTransport());
	    auto streamdecks = mngr.enumerate();

        for (auto deck : streamdecks)
        {
            deck->open();
            deck->reset();

            Profile profile = loadProfile(deck->get_serial_number());
            auto reg_device = std::make_shared<RegisteredDevice>(deck, profile);
            reg_device->init(m_module_loader);
            m_registered_deices.push_back(reg_device);
        }
    }

    void start()
    {
        for ( ;; )
        {
            for (auto device : m_registered_deices)
            {
                if (device->is_device_open())
                {
                    device->tick();
                }
            }

            std::this_thread::sleep_for(100ms);
            std::this_thread::yield();
        }
    }
protected:
    std::shared_ptr<ModuleLoader> m_module_loader;
    std::vector<std::shared_ptr<RegisteredDevice>> m_registered_deices;
};