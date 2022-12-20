#include "StreamDeckFactory.h"


namespace
{

template <typename T>
std::shared_ptr<BaseStreamDeck> createStreamDeckCommon(std::shared_ptr<IDevice> device)
{
    return std::make_shared<T>(device);
}

}

template <typename T>
struct StreamDeckRegistrator : private StreamDeckFactory
{
    static void insertDeckProduct(USBProductIDs s)
    {
        getMap()->insert(std::make_pair(s, &createStreamDeckCommon<T>));
    }
};
