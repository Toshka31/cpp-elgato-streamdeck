#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>


class IStreamDeck
{
public:
    typedef std::function<void(std::shared_ptr<IStreamDeck>, unsigned short, bool)> KeyCallback;

    IStreamDeck() = default;

    virtual ~IStreamDeck()  = default;

    struct KeyImageFormat {
        std::pair<unsigned short, unsigned short> size;
        std::string format;
        std::pair<bool, bool> flip;
        unsigned short rotation;
    };

    virtual void reset() = 0;

    virtual void set_brightness(unsigned char percent) = 0;

    virtual std::string get_serial_number() = 0;

    virtual std::string get_firmware_version() = 0;

    virtual void set_key_image(unsigned char key_index, std::vector<unsigned char> image) = 0;

    virtual void open() = 0;

    virtual void close() = 0;

    virtual bool is_open() const = 0;

    virtual bool connected() const = 0;

    virtual unsigned short vendor_id() const = 0;

    virtual unsigned short product_id() const = 0;

    virtual std::string id()  const = 0;

    virtual unsigned short key_count() const = 0;

    virtual std::string deck_type() const = 0;

    virtual bool is_visual() const = 0;

    virtual std::pair<unsigned short, unsigned short> key_layout() const = 0;

    virtual KeyImageFormat key_image_format() const = 0;

    virtual void set_poll_frequency(unsigned int hz) = 0;

    virtual void set_key_callback(KeyCallback callback)  = 0;

    virtual std::vector<bool> key_states() const = 0;
};