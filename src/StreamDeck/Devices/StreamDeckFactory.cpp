#include "StreamDeckFactory.h"

#include <stdexcept>


StreamDeckFactory::map_type * StreamDeckFactory::map = nullptr;

std::shared_ptr<IStreamDeck> StreamDeckFactory::createInstance(USBProductIDs s, std::shared_ptr<IDevice> device)
{
    auto it = getMap()->find(s);
    if (it == getMap()->end())
        throw std::runtime_error("StreamDeckFactory::createInstance: no suitable types for factory");

    return it->second(device);
}

StreamDeckFactory::map_type * StreamDeckFactory::getMap() 
{
    if (!map)
        map = new map_type;

    return map; 
}
