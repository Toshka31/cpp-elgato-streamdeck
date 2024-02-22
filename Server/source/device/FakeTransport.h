#ifndef ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKETRANSPORT_H_
#define ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKETRANSPORT_H_

#include <device/ITransport.h>

class FakeTransport : public ITransport
{
 public:
  std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid) override;
};

#endif //ELGATO_STREAMDECK_SRC_STREAMDECK_TRANSPORT_FAKETRANSPORT_H_
