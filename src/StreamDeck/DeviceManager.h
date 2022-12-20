#pragma once

#include <vector>
#include <memory>


class ITransport;
class BaseStreamDeck;

/*
    Central device manager, to enumerate any attached StreamDeck devices. An
    instance of this class must be created in order to detect and use any
    StreamDeck devices.
*/
class DeviceManager
{
public:
    DeviceManager(std::shared_ptr<ITransport> transport)
        : m_transport(transport) {}

    std::vector<std::shared_ptr<BaseStreamDeck>> enumerate();

private:
    std::shared_ptr<ITransport> m_transport;
};
