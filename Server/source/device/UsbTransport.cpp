#include "UsbTransport.h"

std::vector<std::shared_ptr<IDevice>> UsbTransport::enumerate(unsigned short vid, unsigned short pid)
{
    auto lib_usb = std::make_shared<LibUSBHIDWrapper>();
    std::vector<std::shared_ptr<IDevice>> ret_devices;
    const auto devices = lib_usb->enumerate(vid, pid);
    for (auto device : devices)
    {
        ret_devices.emplace_back(std::make_shared<UsbDevice>(lib_usb, device));
    }
    return ret_devices;
}