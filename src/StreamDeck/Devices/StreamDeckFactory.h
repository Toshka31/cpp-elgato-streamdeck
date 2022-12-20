#pragma once

#include "../ProductID.h"

#include <memory>
#include <map>


class BaseStreamDeck;
class IDevice;

struct StreamDeckFactory 
{
    static std::shared_ptr<BaseStreamDeck> createInstance(USBProductIDs s, std::shared_ptr<IDevice> device);

protected:
    using map_type = std::map<unsigned short, std::shared_ptr<BaseStreamDeck>(*)(std::shared_ptr<IDevice>)>;

    static map_type * getMap();

private:
    static map_type* map;
};
