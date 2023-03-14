#pragma once

#include <StreamDeckLib/Transport/IDevice.h>

#include <memory>
#include <vector>


/*
Base transport layer, representing an abstract communication back-end which
can be used to discovery attached StreamDeck devices.
*/
class ITransport
{
public:  
    /*
    Enumerates all available devices on the system using the current
    transport back-end.
    :param int vid: USB Vendor ID to filter all devices by, `None` if the
                    device list should not be filtered by vendor.
    :param int pid: USB Product ID to filter all devices by, `None` if the
                    device list should not be filtered by product.
    :rtype: list(Mediator.Device)
    :return: List of discovered devices that are available through this
                transport back-end.
    */
    virtual std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid) = 0;
};
