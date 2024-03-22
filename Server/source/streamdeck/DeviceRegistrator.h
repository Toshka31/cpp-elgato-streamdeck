#include "StreamDeckFactory.h"

template<typename T>
struct StreamDeckRegistrator : public StreamDeckFactory {
    explicit StreamDeckRegistrator(USBProductIDs s) {
        getMap()->insert(std::make_pair(s, &createStreamDeck<T>));
    }
};