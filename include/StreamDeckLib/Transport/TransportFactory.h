#pragma once

#include <StreamDeckLib/Transport/ITransport.h>


class TransportFactory
{
public:
    static std::shared_ptr<ITransport> createUsbTransport();
};