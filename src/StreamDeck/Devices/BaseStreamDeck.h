#pragma once

#include "../Transport/ITransport.h"
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
    );

    virtual ~BaseStreamDeck();

    virtual void reset() = 0;

    virtual void set_brightness(unsigned char percent) = 0;

    virtual std::string get_serial_number() = 0;

    virtual std::string get_firmware_version() = 0;

    virtual void set_key_image(unsigned char key_index, std::vector<unsigned char> image) = 0;

    void open();

    void close();

    bool is_open() const;

    bool connected() const;

    unsigned short vendor_id() const;

    unsigned short product_id()  const;

    std::string id()  const;

    unsigned short key_count()  const;

    std::string deck_type()  const;

    bool is_visual()  const;

    std::pair<unsigned short, unsigned short> key_layout()  const;

    struct KeyImageFormat {
        std::pair<unsigned short, unsigned short> size;
        std::string format;
        std::pair<bool, bool> flip;
        unsigned short rotation;
    };

    KeyImageFormat key_image_format()  const;

    void set_poll_frequency(unsigned int hz);

    void set_key_callback(std::function<void(std::shared_ptr<BaseStreamDeck>, unsigned short, bool)> callback);

    std::vector<bool> key_states() const;

protected:
    virtual std::vector<bool> read_key_states() = 0;

    virtual void reset_key_stream() = 0;

    void read();

    void setup_reader(std::function<void()> callback);

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
    mutable std::mutex key_state_mutex;
};