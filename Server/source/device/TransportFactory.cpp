#include "FakeTransport.h"
#include "UsbTransport.h"
#include <device/TransportFactory.h>

std::shared_ptr<ITransport> TransportFactory::createUsbTransport() {
    return std::make_shared<UsbTransport>();
}

std::shared_ptr<ITransport> TransportFactory::createDebugTransport() {
    return std::make_shared<FakeTransport>();
}