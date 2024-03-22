#pragma once

#include "ITransport.h"

class TransportFactory {
public:
    static std::shared_ptr<ITransport> createUsbTransport();

    static std::shared_ptr<ITransport> createDebugTransport();
};