#pragma once

#include <string>
#include <vector>
#include <memory>


class IStreamDeck;

class IComponent
{
public:
    virtual std::string name() const = 0;

    virtual std::vector<unsigned char> getImage() const = 0;

    virtual void init(std::shared_ptr<IStreamDeck> deck) = 0;

    virtual void tick() = 0;

    virtual void actionPress(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

    virtual void actionRelease(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

    virtual ~IComponent() {}
};
