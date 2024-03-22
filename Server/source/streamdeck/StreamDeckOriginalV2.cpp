#include "StreamDeckOriginalV2.h"

#include <utility>

StreamDeckRegistrator<StreamDeckOriginalV2> StreamDeckOriginalV2::reg(USBProductIDs::USB_PID_STREAMDECK_ORIGINAL_V2);

StreamDeckOriginalV2::StreamDeckOriginalV2(std::shared_ptr<IDevice> device)
    : BaseStreamDeck(std::move(device), 15, 5, 3, 72, 72, "JPEG", { true, true }, 0, "Stream Deck Original", true)
{}

void StreamDeckOriginalV2::reset() {
    std::vector<unsigned char> payload(32);
    payload[0] = 0x03;
    payload[1] = 0x02;
    m_device->write_feature(payload);
}

void StreamDeckOriginalV2::set_brightness(unsigned char percent) {
    if (percent > 100)
        percent = 100;

    std::vector<unsigned char> payload(32);
    payload[0] = 0x03;
    payload[1] = 0x08;
    payload[2] = percent;
    m_device->write_feature(payload);
}

std::string StreamDeckOriginalV2::get_serial_number() {
    std::vector<unsigned char> data = m_device->read_feature(0x06, 32);
    std::string serial(data.begin() + 2, data.end());
    serial.erase(std::find_if(serial.rbegin(), serial.rend(),
                              std::not_fn([](auto && PH1) { return std::iscntrl(std::forward<decltype(PH1)>(PH1)); })).base(), serial.end());
    return serial;
}

std::string StreamDeckOriginalV2::get_firmware_version() {
    std::vector<unsigned char> data = m_device->read_feature(0x05, 32);
    std::string firmware(data.begin() + 6, data.end());
    return firmware;
}

void StreamDeckOriginalV2::set_key_image(unsigned char key_index, std::vector<unsigned char> image_data) {
    if (key_index > m_key_count)
        return;

    if (image_data.empty())
        image_data = BLANK_KEY_IMAGE;

    unsigned short page_number = 0;
    unsigned short bytes_remaining = image_data.size();
    while (bytes_remaining > 0) {
        unsigned short this_length = std::min(bytes_remaining, IMAGE_REPORT_PAYLOAD_LENGTH);
        unsigned short bytes_sent = page_number * IMAGE_REPORT_PAYLOAD_LENGTH;

        std::vector<unsigned char> payload(IMAGE_REPORT_LENGTH, 0x00);
        // header
        payload[0] = 0x02;
        payload[1] = 0x07;
        payload[2] = key_index;
        payload[3] = (this_length == bytes_remaining) ? 0x01 : 0x00;
        payload[4] = this_length & 0xFF;
        payload[5] = this_length >> 8;
        payload[6] = page_number & 0xFF;
        payload[7] = page_number >> 8;

        std::copy(image_data.begin() + bytes_sent,
                  image_data.begin() + bytes_sent + this_length,
                  payload.begin() + IMAGE_REPORT_HEADER_LENGTH
        );
        m_device->write(payload);

        bytes_remaining -= this_length;
        page_number++;
    }
}

std::vector<bool> StreamDeckOriginalV2::read_key_states() {
    std::vector<bool> key_states;
    std::vector<unsigned char> data = m_device->read(4 + m_key_count);

    if (!data.empty()) {
        key_states.resize(m_key_count);
        std::copy(data.begin() + 4, data.end(), key_states.begin());
    }

    return key_states;
}

void StreamDeckOriginalV2::reset_key_stream() {
    std::vector<unsigned char> payload(IMAGE_REPORT_LENGTH);
    payload[0] = 0x02;
    m_device->write(payload);
}
