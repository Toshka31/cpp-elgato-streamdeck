
#include "BaseStreamDeck.h"

#include <memory>

template<typename T>
std::shared_ptr<BaseStreamDeck> createStreamDeck(std::shared_ptr<IDevice> device)
{
    return std::make_shared<T>(device);
}

struct StreamDeckFactory 
{
    typedef std::map<unsigned short, std::shared_ptr<BaseStreamDeck>(*)(std::shared_ptr<IDevice>)> map_type;

    static std::shared_ptr<BaseStreamDeck> createInstance(USBProductIDs s, std::shared_ptr<IDevice> device);

protected:
    static map_type * getMap();

private:
    static map_type* map;
};