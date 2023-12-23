#include "FakeDevice.h"

FakeDevice::FakeDevice() {
  m_sock_out = zmq::socket_t(m_ctx, zmq::socket_type::push);
  m_sock_in = zmq::socket_t(m_ctx, zmq::socket_type::pull);
  m_sock_out.bind("tcp://127.0.0.1:33634");
  m_sock_in.connect("tcp://127.0.0.1:33633");
}

FakeDevice::~FakeDevice() {

}

void FakeDevice::open() {

}

void FakeDevice::close() {

}

bool FakeDevice::is_open() const {
  return true;
}

bool FakeDevice::connected() const {
  return true;
}

unsigned short FakeDevice::vendor_id() const {
  return 0;
}

unsigned short FakeDevice::product_id() const {
  return 0;
}

std::string FakeDevice::path() const {
  return std::string();
}

int FakeDevice::write_feature(std::vector<unsigned char> payload) {
  return 0;
}

std::vector<unsigned char> FakeDevice::read_feature(unsigned char report_id, size_t length) {
  if (report_id == 0x05) {
    return std::vector<unsigned char>(length, '0');
  }
  else if (report_id == 0x06)
    return std::vector<unsigned char>(length, 'F');
}

int FakeDevice::write(std::vector<unsigned char> data) {
  zmq::message_t msg(data.begin(), data.end());
  m_sock_out.send(msg, zmq::send_flags::none);
  return 0;
}

std::vector<unsigned char> FakeDevice::read(size_t length) {
  zmq::message_t msg;
  auto opt_size = m_sock_in.recv(msg);
  if (opt_size.has_value() && opt_size.value() == length) {
    std::vector<unsigned char> ret(msg.data<unsigned char>(), msg.data<unsigned char>() + length);
    return ret;
  }
  return std::vector<unsigned char>();
}

