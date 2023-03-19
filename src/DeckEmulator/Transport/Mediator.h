#pragma once

#include "MediatorInterface.h"

#include <zmq.hpp>

#include <mutex>
#include <thread>
#include <vector>


template <typename DeckWidget>
class Mediator : public MediatorInterface
{
public:
    Mediator();
    ~Mediator();

public:
    void buttonPress(int index) override;
    void buttonRelease(int index) override;

private:
    void sendStateLoop();
    void receiveLoop();

private:
    std::thread m_worker_sender;
    std::thread m_worker_receiver;

    zmq::context_t m_ctx;
    zmq::socket_t m_sock_out;
    zmq::socket_t m_sock_in;

    std::vector<unsigned char> m_cached_data;
    std::vector<unsigned char> m_button_states;

    std::mutex m_mutex;
    bool m_is_running;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename DeckWidget>
Mediator<DeckWidget>::Mediator()
    : m_sock_out(zmq::socket_t(m_ctx, zmq::socket_type::push))
    , m_sock_in(zmq::socket_t(m_ctx, zmq::socket_type::pull))
    , m_is_running(true)
{
    m_button_states.resize(DeckWidget::BUTTONS_NUM_ROWS * DeckWidget::BUTTONS_NUM_COLS
                           + DeckWidget::KEY_STATES_PAYLOAD_OFFSET);

    m_sock_out.bind("tcp://127.0.0.1:33633");
    m_sock_in.connect("tcp://127.0.0.1:33634");

    m_worker_sender = std::thread(std::bind(&Mediator::sendStateLoop, this));
    m_worker_receiver = std::thread(std::bind(&Mediator::receiveLoop, this));
}

template <typename DeckWidget>
Mediator<DeckWidget>::~Mediator()
{
    m_is_running = false;
    m_worker_sender.join();
    m_worker_receiver.join();
    m_sock_in.close();
    m_sock_out.close();
}

template <typename DeckWidget>
void Mediator<DeckWidget>::sendStateLoop()
{
    using namespace std::chrono_literals;

    while (m_is_running)
    {
        zmq::message_t msg;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            msg = zmq::message_t(m_button_states.begin(), m_button_states.end());
        }
        m_sock_out.send(msg, zmq::send_flags::none);
        std::this_thread::sleep_for(100ms);
    }
}

template <typename DeckWidget>
void Mediator<DeckWidget>::receiveLoop()
{
    using namespace std::chrono_literals;

    while (m_is_running)
    {
        zmq::message_t msg;
        auto opt_size = m_sock_in.recv(msg, zmq::recv_flags::dontwait);
        if (opt_size.has_value() && opt_size.value())
        {
            if (msg.data<uchar>()[0] == 0x02 && msg.data<uchar>()[1] == 0x07)
            {
                m_cached_data.insert(m_cached_data.end(), msg.data<uchar>() + DeckWidget::IMAGE_PAYLOAD_OFFSET,
                                     msg.data<uchar>() + msg.size());
                if (msg.data<uchar>()[3])
                {
                    emit buttonImage(msg.data<uchar>()[2], m_cached_data);
                }
            }
        }
        std::this_thread::sleep_for(100ms);
    }
}

template <typename DeckWidget>
void Mediator<DeckWidget>::buttonPress(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_button_states[index + DeckWidget::KEY_STATES_PAYLOAD_OFFSET] = 1;
}

template <typename DeckWidget>
void Mediator<DeckWidget>::buttonRelease(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_button_states[index + DeckWidget::KEY_STATES_PAYLOAD_OFFSET] = 0;
}
