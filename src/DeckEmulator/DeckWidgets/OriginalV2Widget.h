#ifndef ELGATO_STREAMDECK_SRC_DECKEMULATOR_DECKWIDGETS_ORIGINALV2WIDGET_H_
#define ELGATO_STREAMDECK_SRC_DECKEMULATOR_DECKWIDGETS_ORIGINALV2WIDGET_H_

#include <thread>
#include <QWidget>
#include <QPushButton>

#include <QMap>
#include <zmq.hpp>
#include <mutex>

class Mediator : public QObject {
 Q_OBJECT
 public:
  Mediator();
  ~Mediator();

  void buttonPress(int index);
  void buttonRelease(int index);
 signals:
  void signalSetButtonImage(int index, std::vector<unsigned char> data);
 private:
  void sendStateLoop();
  void receiveLoop();
 private:
  bool is_running = true;

  std::thread m_thread_send;
  std::thread m_thread_recieve;

  zmq::context_t m_ctx;
  zmq::socket_t m_sock_out;
  zmq::socket_t m_sock_in;

  std::vector<unsigned char> m_cached_data;

  std::mutex m_mutex;
  std::vector<unsigned char> m_button_states;
};

class OriginalV2Widget : public QWidget
{
 Q_OBJECT
 public:
  explicit OriginalV2Widget(QWidget *parent = 0);
 public slots:
  void buttonPressed();
  void buttonReleased();
  void buttonChangeImage(int index, std::vector<unsigned char> data);
 private:
  QMap<int, QPushButton*> m_buttons;

  Mediator m_transport;
};
#endif //ELGATO_STREAMDECK_SRC_DECKEMULATOR_DECKWIDGETS_ORIGINALV2WIDGET_H_
