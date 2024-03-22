#include "FakeTransport.h"
#include "FakeDevice.h"

#include "StreamDeckLib/ProductID.h"

std::vector<std::shared_ptr<IDevice>> FakeTransport::enumerate(unsigned short vid, unsigned short pid) {
    std::vector<std::shared_ptr<IDevice>> ret_devices;
    if (pid == USBProductIDs::USB_PID_STREAMDECK_ORIGINAL_V2) {
        std::shared_ptr<FakeDevice> device = std::make_shared<FakeDevice>();
        ret_devices.push_back(device);
    }
    return ret_devices;
}
