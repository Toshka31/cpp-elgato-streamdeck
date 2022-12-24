#pragma once

#include <boost/config.hpp>
#include <string>
#include <vector>
#include <memory>

class IComponent;

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
   virtual std::string name() const = 0;

   virtual void actionPress() = 0;

   virtual void actionRelease() = 0;

   virtual ~IComponent() {}
};