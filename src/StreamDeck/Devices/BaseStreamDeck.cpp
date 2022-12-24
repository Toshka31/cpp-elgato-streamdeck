#include "BaseStreamDeck.h"

#include <StreamDeckLib/Transport/ITransport.h>


BaseStreamDeck::BaseStreamDeck(
    std::shared_ptr<IDevice> device, 
    unsigned short KEY_COUNT, 
    unsigned short KEY_COLS,
    unsigned short KEY_ROWS ,
    unsigned short KEY_PIXEL_WIDTH,
    unsigned short KEY_PIXEL_HEIGHT,
    std::string KEY_IMAGE_FORMAT,
    std::pair<bool, bool> KEY_FLIP,
    unsigned short KEY_ROTATION,
    std::string DECK_TYPE,
    bool DECK_VISUAL
) : m_device(device),
    KEY_COUNT(KEY_COUNT),
    KEY_COLS(KEY_COLS),
    KEY_ROWS(KEY_ROWS) ,
    KEY_PIXEL_WIDTH(KEY_PIXEL_WIDTH),
    KEY_PIXEL_HEIGHT(KEY_PIXEL_HEIGHT),
    KEY_IMAGE_FORMAT(KEY_IMAGE_FORMAT),
    KEY_FLIP(KEY_FLIP),
    KEY_ROTATION(KEY_ROTATION),
    DECK_TYPE(DECK_TYPE),
    DECK_VISUAL(DECK_VISUAL)
{
    last_key_stated = std::vector<bool>(KEY_COUNT, false);
}

BaseStreamDeck::~BaseStreamDeck() 
{
    setup_reader({});
    close();
}

void BaseStreamDeck::open()
{
    m_device->open();

    reset_key_stream();
    setup_reader(std::bind(&BaseStreamDeck::read, this));
}

void BaseStreamDeck::close()
{
    m_device->close();
}

bool BaseStreamDeck::is_open() const
{
    return m_device->is_open();
}

bool BaseStreamDeck::connected() const
{
    return m_device->connected();
}

unsigned short BaseStreamDeck::vendor_id() const
{
    return m_device->vendor_id();
}

unsigned short BaseStreamDeck::product_id() const
{
    return m_device->product_id();
}

std::string BaseStreamDeck::id() const
{
    return m_device->path();
}  

unsigned short BaseStreamDeck::key_count() const
{
    return KEY_COUNT;
}

std::string BaseStreamDeck::deck_type() const
{
    return DECK_TYPE;
}

bool BaseStreamDeck::is_visual() const
{
    return DECK_VISUAL;
}

std::pair<unsigned short, unsigned short> BaseStreamDeck::key_layout() const
{
    return {KEY_ROWS, KEY_COLS};
}

BaseStreamDeck::KeyImageFormat BaseStreamDeck::key_image_format() const
{
    return {{KEY_PIXEL_WIDTH, KEY_PIXEL_HEIGHT}, KEY_IMAGE_FORMAT, KEY_FLIP, KEY_ROTATION};
}

void BaseStreamDeck::set_poll_frequency(unsigned int hz) 
{
    read_poll_hz = std::min(std::max(hz, 1U), 1000U);
}

void BaseStreamDeck::set_key_callback(IStreamDeck::KeyCallback callback)
{
    std::lock_guard<std::mutex> guard(key_state_mutex);
    key_callback = callback;
}

std::vector<bool> BaseStreamDeck::key_states() const
{
    std::lock_guard<std::mutex> guard(key_state_mutex);
    return last_key_stated;
}

void BaseStreamDeck::read()
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

void BaseStreamDeck::setup_reader(std::function<void()> callback)
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
