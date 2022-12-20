#include "StreamDeckFactory.h"


template <typename T>
std::shared_ptr<BaseStreamDeck> createStreamDeck(std::shared_ptr<IDevice> device)
{
    return std::make_shared<T>(device);
}

template <typename T>
struct StreamDeckRegistrator : private StreamDeckFactory
{
    StreamDeckRegistrator(USBProductIDs s) 
    {
        getMap()->insert(std::make_pair(s, &createStreamDeck<T>));
    }
};
