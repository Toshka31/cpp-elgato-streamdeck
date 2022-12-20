#include "StreamDeckFactory.h"


StreamDeckFactory::map_type * StreamDeckFactory::map = nullptr;

std::shared_ptr<BaseStreamDeck> StreamDeckFactory::createInstance(USBProductIDs s, std::shared_ptr<IDevice> device) 
{
    auto it = getMap()->find(s);
    if (it == getMap()->end())
        return std::shared_ptr<BaseStreamDeck>();

    return it->second(device);
}

StreamDeckFactory::map_type * StreamDeckFactory::getMap() 
{
    if (!map)
        map = new map_type;

    return map; 
}
