#include <string>
#include <iostream>

#include <StreamDeckLib/DeviceManager.h>
#include <StreamDeckLib/ProductID.h>
#include <StreamDeckLib/Transport/ITransport.h>
#include "Devices/StreamDeckFactory.h"


DeviceManager::DeviceManager(std::shared_ptr<ITransport> transport)
    : m_transport(transport) {}

std::vector<std::shared_ptr<IStreamDeck>> DeviceManager::enumerate() 
{
    static std::vector<USBProductIDs> products = {
        USBProductIDs::USB_PID_STREAMDECK_ORIGINAL,
        USBProductIDs::USB_PID_STREAMDECK_ORIGINAL_V2,
        USBProductIDs::USB_PID_STREAMDECK_MINI,
        USBProductIDs::USB_PID_STREAMDECK_XL,
        USBProductIDs::USB_PID_STREAMDECK_MK2,
        USBProductIDs::USB_PID_STREAMDECK_PEDAL,
        USBProductIDs::USB_PID_STREAMDECK_MINI_MK2,
        USBProductIDs::USB_PID_STREAMDECK_XL_V2,
    };

    std::vector<std::shared_ptr<IStreamDeck>> list_streamdeck;
    for (auto prod : products)
    {
        auto devices = m_transport->enumerate(USBVendorIDs::USB_VID_ELGATO, prod);
        for (const auto &device : devices)
        {
            std::cout << prod << std::endl;
            list_streamdeck.push_back(StreamDeckFactory::createInstance(prod, device));
        }
    }

    return list_streamdeck;
}
