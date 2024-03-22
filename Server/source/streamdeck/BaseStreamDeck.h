#pragma once

#include <streamdeck/IStreamDeck.h>
#include <device/ITransport.h>
#include <device/IDevice.h>
#include <StreamDeckLib//ProductID.h>

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

class BaseStreamDeck : public std::enable_shared_from_this<BaseStreamDeck>, public IStreamDeck {
public:
    BaseStreamDeck(
        std::shared_ptr<IDevice> device,
        unsigned short key_count = 0,
        unsigned short key_cols = 0,
        unsigned short key_rows = 0,
        unsigned short key_image_pixel_width = 0,
        unsigned short key_image_pixel_height = 0,
        std::string key_image_format = "",
        std::pair<bool, bool> key_flip = { false, false },
        unsigned short key_rotation = 0,
        std::string deck_type = "",
        bool deck_visual = false
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

    unsigned short product_id() const;

    std::string id() const;

    unsigned short key_count() const;

    std::string deck_type() const;

    bool is_visual() const;

    std::pair<unsigned short, unsigned short> key_layout() const;

    KeyImageFormat key_image_format() const;

    void set_poll_frequency(unsigned int hz);

    void set_key_callback(IStreamDeck::KeyCallback callback);

    std::vector<bool> key_states() const;

protected:
    virtual std::vector<bool> read_key_states() = 0;

    virtual void reset_key_stream() = 0;

    void read();

    void setup_reader(std::function<void()> callback);

protected:
    unsigned short m_key_count;
    unsigned short m_key_cols;
    unsigned short m_key_rows;

    unsigned short m_key_image_pixel_width;
    unsigned short m_key_image_pixel_height;

    std::string m_key_image_format;
    std::pair<bool, bool> m_key_image_flip;
    unsigned short m_key_image_rotation;

    std::string m_deck_type;
    bool m_deck_visual;

    std::shared_ptr<IDevice> m_device;

    std::vector<bool> m_last_key_states;

    unsigned int m_read_poll_hz = 20U;

    std::function<void(std::shared_ptr<BaseStreamDeck>, unsigned short, bool)> m_key_callback;

    bool m_run_read_thread = false;
    std::thread m_read_thread;
    mutable std::mutex m_key_state_mutex;
};