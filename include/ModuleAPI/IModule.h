#pragma once

#include <boost/config.hpp>
#include <string>
#include <vector>
#include <memory>

class IStreamDeck; 
class IComponent;
namespace image::helper {
    enum class EImageFormat;
}

#define MAKE_MODULE_FACTORY(ModuleName) \
    static std::shared_ptr<IModule> create() { \
        return std::make_shared<ModuleName>(); \
    }

class BOOST_SYMBOL_VISIBLE IModule 
{
public:
    virtual std::string name() const = 0;

    virtual std::vector<std::shared_ptr<IComponent>> getComponents() const = 0;

    virtual ~IModule() {}
};

class IComponent
{
public:
    virtual void init(std::shared_ptr<IStreamDeck> deck) = 0;

    virtual std::string name() const = 0;

    virtual void actionPress(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

    virtual void actionRelease(std::shared_ptr<IStreamDeck> deck, ushort key) = 0;

    virtual std::vector<unsigned char> getImage(image::helper::EImageFormat &format) const = 0;

    virtual ~IComponent() {}
};