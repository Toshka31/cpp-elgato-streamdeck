#include "BaseStreamDeck.h"

#include <utility>

BaseStreamDeck::BaseStreamDeck(
    std::shared_ptr<IDevice> device,
    unsigned short key_count,
    unsigned short key_cols,
    unsigned short key_rows,
    unsigned short key_image_pixel_width,
    unsigned short key_image_pixel_height,
    std::string key_image_format,
    std::pair<bool, bool> key_flip,
    unsigned short key_rotation,
    std::string deck_type,
    bool deck_visual
)
    : m_device(std::move(device))
    , m_key_count(key_count)
    , m_key_cols(key_cols)
    , m_key_rows(key_rows)
    , m_key_image_pixel_width(key_image_pixel_width)
    , m_key_image_pixel_height(key_image_pixel_height)
    , m_key_image_format(std::move(key_image_format))
    , m_key_image_flip(std::move(key_flip))
    , m_key_image_rotation(key_rotation)
    , m_deck_type(std::move(deck_type))
    , m_deck_visual(deck_visual)
{
    m_last_key_states = std::vector<bool>(key_count, false);
}

BaseStreamDeck::~BaseStreamDeck() {
    setup_reader({});
    close();
}

void BaseStreamDeck::open() {
    m_device->open();

    reset_key_stream();
    setup_reader([this] { read(); });
}

void BaseStreamDeck::close() {
    m_device->close();
}

bool BaseStreamDeck::is_open() const {
    return m_device->is_open();
}

bool BaseStreamDeck::connected() const {
    return m_device->connected();
}

unsigned short BaseStreamDeck::vendor_id() const {
    return m_device->vendor_id();
}

unsigned short BaseStreamDeck::product_id() const {
    return m_device->product_id();
}

std::string BaseStreamDeck::id() const {
    return m_device->path();
}

unsigned short BaseStreamDeck::key_count() const {
    return m_key_count;
}

std::string BaseStreamDeck::deck_type() const {
    return m_deck_type;
}

bool BaseStreamDeck::is_visual() const {
    return m_deck_visual;
}

std::pair<unsigned short, unsigned short> BaseStreamDeck::key_layout() const {
    return { m_key_rows, m_key_cols };
}

BaseStreamDeck::KeyImageFormat BaseStreamDeck::key_image_format() const {
    return {{ m_key_image_pixel_width, m_key_image_pixel_height }, m_key_image_format, m_key_image_flip, m_key_image_rotation };
}

void BaseStreamDeck::set_poll_frequency(unsigned int hz) {
    m_read_poll_hz = std::min(std::max(hz, 1U), 1000U);
}

void BaseStreamDeck::set_key_callback(IStreamDeck::KeyCallback callback) {
    std::lock_guard<std::mutex> guard(m_key_state_mutex);
    m_key_callback = callback;
}

std::vector<bool> BaseStreamDeck::key_states() const {
    std::lock_guard<std::mutex> guard(m_key_state_mutex);
    return m_last_key_states;
}

void BaseStreamDeck::read() {
    using namespace std::chrono_literals;
    while (m_run_read_thread) {
        auto new_key_states = read_key_states();
        if (new_key_states.empty()) {
            std::this_thread::sleep_for(100ms);
            continue;
        }

        std::lock_guard<std::mutex> lock(m_key_state_mutex);
        if (m_key_callback) {
            for (int index = 0; index < m_key_count; ++index)
                if (new_key_states[index] != m_last_key_states[index])
                    m_key_callback(shared_from_this(), index, new_key_states[index]);
        }

        m_last_key_states = new_key_states;
    }
}

void BaseStreamDeck::setup_reader(std::function<void()> callback) {
    m_run_read_thread = false;
    if (m_read_thread.joinable())
        m_read_thread.join();

    if (callback) {
        m_run_read_thread = true;
        m_read_thread = std::thread(callback);
    }
}