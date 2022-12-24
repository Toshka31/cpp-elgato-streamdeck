#pragma once

#include <StreamDeckLib/ProductID.h>
#include <StreamDeckLib/Transport/ITransport.h>
#include <StreamDeckLib/Device/IStreamDeck.h>

#include <vector>


/*
    Central device manager, to enumerate any attached StreamDeck devices. An
    instance of this class must be created in order to detect and use any
    StreamDeck devices.
*/
class DeviceManager
{
public:
    DeviceManager(std::shared_ptr<ITransport> transport);

    std::vector<std::shared_ptr<IStreamDeck>> enumerate();
private:
    std::shared_ptr<ITransport> m_transport;
};
