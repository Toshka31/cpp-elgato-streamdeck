#pragma once

#include "../Transport/Transport.h"
#include "../ProductID.h"

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>


class BaseStreamDeck : public std::enable_shared_from_this<BaseStreamDeck>
{
public:
    BaseStreamDeck(
        std::shared_ptr<IDevice> device, 
        unsigned short KEY_COUNT = 0, 
        unsigned short KEY_COLS = 0,
        unsigned short KEY_ROWS = 0,
        unsigned short KEY_PIXEL_WIDTH = 0,
        unsigned short KEY_PIXEL_HEIGHT = 0,
        std::string KEY_IMAGE_FORMAT = "",
        std::pair<bool, bool> KEY_FLIP = {false, false},
        unsigned short KEY_ROTATION = 0,
        std::string DECK_TYPE = "",
        bool DECK_VISUAL = false
    ) : m_device(device) 
    {
        last_key_stated = std::vector<bool>(KEY_COUNT, false);
    }

    virtual ~BaseStreamDeck() 
    {
        setup_reader({});
        close();
    }

    void open()
    {
        m_device->open();

        reset_key_stream();
        setup_reader(std::bind(&BaseStreamDeck::read, this));
    }

    void close()
    {
        m_device->close();
    }

    bool is_open()
    {
        return m_device->is_open();
    }

    bool connected()
    {
        return m_device->connected();
    }

    unsigned short vendor_id()
    {
        return m_device->vendor_id();
    }

    unsigned short product_id()
    {
        return m_device->product_id();
    }

    std::string id()
    {
        return m_device->path();
    }  

    unsigned short key_count() 
    {
        return KEY_COUNT;
    }

    std::string deck_type()
    {
        return DECK_TYPE;
    }

    bool is_visual()
    {
        return DECK_VISUAL;
    }

    std::pair<unsigned short, unsigned short> key_layout()
    {
        return {KEY_ROWS, KEY_COLS};
    }

    struct KeyImageFormat {
        std::pair<unsigned short, unsigned short> size;
        std::string format;
        std::pair<bool, bool> flip;
        unsigned short rotation;
    };

    KeyImageFormat key_image_format()
    {
        return {{KEY_PIXEL_WIDTH, KEY_PIXEL_HEIGHT}, KEY_IMAGE_FORMAT, KEY_FLIP, KEY_ROTATION};
    }

    void set_poll_frequency(unsigned int hz)
    {
        read_poll_hz = std::min(std::max(hz, 1U), 1000U);
    }

    void set_key_callback(std::function<void(std::shared_ptr<BaseStreamDeck>, unsigned short, bool)> callback)
    {
        std::lock_guard<std::mutex> guard(key_state_mutex);
        key_callback = callback;
    }

    std::vector<bool> key_states()
    {
        std::lock_guard<std::mutex> guard(key_state_mutex);
        return last_key_stated;
    }

    virtual void reset() = 0;
    virtual void set_brightness(unsigned char percent) = 0;
    virtual std::string get_serial_number() = 0;
    virtual std::string get_firmware_version() = 0;
    virtual void set_key_image(unsigned char key_index, std::vector<unsigned char> image) = 0;

protected:
    virtual std::vector<bool> read_key_states() = 0;
    virtual void reset_key_stream() = 0;

    void read()
    {
        using namespace std::chrono_literals;
        while (run_read_thread)
        {
            auto new_key_states = read_key_states();
            if (new_key_states.empty())
            {
                std::this_thread::sleep_for(100ms);
                continue;
            }

            std::lock_guard<std::mutex> lock(key_state_mutex);
            if (key_callback)
            {
                for (int index = 0; index < KEY_COUNT; ++index)
                    if (new_key_states[index] != last_key_stated[index])
                        key_callback(shared_from_this(), index, new_key_states[index]);
            }

            last_key_stated = new_key_states;
        }
    }

    void setup_reader(std::function<void()> callback)
    {
        run_read_thread = false;
        if (read_thread.joinable())
            read_thread.join();

        if (callback)
        {
            run_read_thread = true;
            read_thread = std::thread(callback);
        }
    }
protected:
    unsigned short KEY_COUNT;
    unsigned short KEY_COLS;
    unsigned short KEY_ROWS;

    unsigned short KEY_PIXEL_WIDTH;
    unsigned short KEY_PIXEL_HEIGHT;

    std::string KEY_IMAGE_FORMAT;
    std::pair<bool, bool> KEY_FLIP;
    unsigned short KEY_ROTATION;

    std::string DECK_TYPE;
    bool DECK_VISUAL;

    std::shared_ptr<IDevice> m_device;

    std::vector<bool> last_key_stated;

    unsigned int read_poll_hz = 20U;

    std::function<void(std::shared_ptr<BaseStreamDeck>, unsigned short, bool)> key_callback;

    bool run_read_thread = false;
    std::thread read_thread;
    std::mutex key_state_mutex;
};

template<typename T>
std::shared_ptr<BaseStreamDeck> createStreamDeck(std::shared_ptr<IDevice> device)
{
    return std::make_shared<T>(device);
}

struct BaseStreamDeckFactory 
{
    typedef std::map<unsigned short, std::shared_ptr<BaseStreamDeck>(*)(std::shared_ptr<IDevice>)> map_type;

    static std::shared_ptr<BaseStreamDeck> createInstance(USBProductIDs s, std::shared_ptr<IDevice> device) {
        map_type::iterator it = getMap()->find(s);
        if(it == getMap()->end())
            return std::shared_ptr<BaseStreamDeck>();
        return it->second(device);
    }

protected:
    static map_type * getMap() {
        if(!map) { map = new map_type; } 
        return map; 
    }

private:
    static map_type* map;
};

template<typename T>
struct StreamDeckRegister : public BaseStreamDeckFactory {
    StreamDeckRegister(USBProductIDs s) {
        getMap()->insert(std::make_pair(s, &createStreamDeck<T>));
    }
};
