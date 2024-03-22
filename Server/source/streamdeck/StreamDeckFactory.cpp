#include "StreamDeckFactory.h"

#include <utility>

StreamDeckFactory::map_type *StreamDeckFactory::map = nullptr;

std::shared_ptr<BaseStreamDeck> StreamDeckFactory::createInstance(USBProductIDs s, std::shared_ptr<IDevice> device) {
    auto it = getMap()->find(s);
    if (it == getMap()->end())
        return {};
    return it->second(std::move(device));
}

StreamDeckFactory::map_type *StreamDeckFactory::getMap() {
    if (!map)
        map = new map_type;
    return map;
}