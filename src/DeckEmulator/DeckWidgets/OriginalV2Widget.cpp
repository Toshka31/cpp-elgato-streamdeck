#include <QGridLayout>

#include "OriginalV2Widget.h"

OriginalV2Widget::OriginalV2Widget(QWidget *parent)
{
  auto *lt_main = new QGridLayout(this);

  for (auto i = 0; i < 3; ++i)
  {
    for (auto j = 0; j < 5; ++j)
    {
      auto *btn = new QPushButton(this);
      btn->setFixedSize(72,72);
      btn->setToolTip(QString::number(i*5+j));
      btn->setIconSize(QSize(70,70));
      QObject::connect(btn, &QPushButton::pressed, this, &OriginalV2Widget::buttonPressed);
      QObject::connect(btn, &QPushButton::released, this, &OriginalV2Widget::buttonReleased);
      m_buttons.insert(i*5+j, btn);
      lt_main->addWidget(btn, i, j);
    }
  }

  setLayout(lt_main);

  QObject::connect(&m_transport, &Mediator::signalSetButtonImage, this, &OriginalV2Widget::buttonChangeImage);
}

void Mediator::buttonPress(int index) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_button_states[index+4] = 1;
}

void Mediator::buttonRelease(int index) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_button_states[index+4] = 0;
}

void OriginalV2Widget::buttonPressed() {
  auto button = qobject_cast<QPushButton *>(sender());
  Q_ASSERT(button);

  QString tooltop = button->toolTip();
  m_transport.buttonPress(tooltop.toInt());
}

void OriginalV2Widget::buttonReleased() {
  auto button = qobject_cast<QPushButton *>(sender());
  Q_ASSERT(button);

  QString tooltop = button->toolTip();
  m_transport.buttonRelease(tooltop.toInt());
}
void OriginalV2Widget::buttonChangeImage(int index, std::vector<unsigned char> data) {
  QPixmap pix;
  pix.loadFromData(data.data(), data.size(), "JPG");
  QTransform myTransform;
  myTransform.rotate(180);
  pix = pix.transformed(myTransform);
  m_buttons[index]->setIcon(QIcon(pix));
}

Mediator::Mediator() {
  m_button_states.resize(19);

  m_sock_out = zmq::socket_t(m_ctx, zmq::socket_type::push);
  m_sock_in = zmq::socket_t(m_ctx, zmq::socket_type::pull);
  m_sock_out.bind("tcp://127.0.0.1:33633");
  m_sock_in.connect("tcp://127.0.0.1:33634");

  is_running = true;
  m_thread_send = std::thread(std::bind(&Mediator::sendStateLoop, this));
  m_thread_recieve = std::thread(std::bind(&Mediator::receiveLoop, this));
}
Mediator::~Mediator() {
  is_running = false;
  m_sock_in.close();
  m_sock_out.close();
  m_thread_send.join();
  m_thread_recieve.join();
}
void Mediator::sendStateLoop() {
  using namespace std::chrono_literals;
  while(is_running)
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
void Mediator::receiveLoop() {
  while(is_running) {
    zmq::message_t msg;
    auto opt_size = m_sock_in.recv(msg);
    if (opt_size.has_value() && opt_size.value()) {
      if (msg.data<uchar>()[0] == 0x02 && msg.data<uchar>()[1] == 0x07) {
        m_cached_data.insert(m_cached_data.end(), msg.data<uchar>() + 8, msg.data<uchar>() + msg.size());
        if (msg.data<uchar>()[3])
            emit signalSetButtonImage(msg.data<uchar>()[2], m_cached_data);
      }
    }
  }
}
