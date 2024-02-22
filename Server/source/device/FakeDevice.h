#ifndef ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKEDEVICE_H_
#define ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKEDEVICE_H_

#include <zmq.hpp>

#include <device/IDevice.h>

class FakeDevice : public IDevice
{
 public:
  FakeDevice();

  ~FakeDevice() override;

  void open() override;

  void close() override;

  bool is_open() const override;

  bool connected() const override;

  unsigned short vendor_id() const override;

  unsigned short product_id() const override;

  std::string path() const override;

  int write_feature(std::vector<unsigned char> payload) override;

  std::vector<unsigned char> read_feature(unsigned char report_id, size_t length) override;

  int write(std::vector<unsigned char> data) override;

  std::vector<unsigned char> read(size_t length) override;
 private:
  zmq::context_t m_ctx;
  zmq::socket_t m_sock_out;
  zmq::socket_t m_sock_in;
};

#endif //ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKEDEVICE_H_
