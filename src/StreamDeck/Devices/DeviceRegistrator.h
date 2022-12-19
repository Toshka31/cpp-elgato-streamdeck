#include "StreamDeckFactory.h"

template<typename T>
struct StreamDeckRegistrator : public StreamDeckFactory 
{
    StreamDeckRegistrator(USBProductIDs s) 
    {
        getMap()->insert(std::make_pair(s, &createStreamDeck<T>));
    }
};