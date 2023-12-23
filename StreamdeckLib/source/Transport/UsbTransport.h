#pragma once

#include <StreamDeckLib/Transport/ITransport.h>
#include "UsbDevice.h"

#include <memory>
#include <vector>


class UsbTransport : public ITransport
{
public:
    std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid) override;
};
