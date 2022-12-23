#include <StreamDeckLib/Transport/TransportFactory.h>
#include "UsbTransport.h"


std::shared_ptr<ITransport> TransportFactory::createUsbTransport()
{
    return std::make_shared<UsbTransport>();
}