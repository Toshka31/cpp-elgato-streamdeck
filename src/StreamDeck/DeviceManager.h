#pragma once

#include <string>
#include <iostream>

#include "Devices/StreamDeckFactory.h"
#include "ProductID.h"


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

    std::vector<std::shared_ptr<BaseStreamDeck>> enumerate() 
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

        std::vector<std::shared_ptr<BaseStreamDeck>> list_streamdeck;
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

private:
    std::shared_ptr<ITransport> m_transport;
};
