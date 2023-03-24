#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

#define DECLARE_MODULE(name) \
class BOOST_SYMBOL_VISIBLE Module##name : public IModule { \
 public:\
    static std::shared_ptr<IModule> create();\
    std::string getName() const override;\
    std::shared_ptr<IComponent> createComponent(const std::string& name) override;\
    std::vector<std::string> getComponentList() const override;\
    struct ComponentRegistrator {\
      ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)());\
    };\
    virtual ~Module##name() = default;\
  private:\
    static std::map<std::string, std::shared_ptr<IComponent>(*)()> components;\
};\
BOOST_DLL_ALIAS(Module##name::create, create);

#define DEFINE_MODULE(name) \
std::shared_ptr<IModule> Module##name::create() { return std::make_shared<Module##name>(); }\
std::string Module##name::getName() const { return #name; }\
std::shared_ptr<IComponent> Module##name::createComponent(const std::string& name) { return Module##name::components[name](); }\
std::vector<std::string> Module##name::getComponentList() const {\
      std::vector<std::string> res;\
      for (const auto &comp : Module##name::components)\
        res.push_back(comp.first);\
      return res;\
    }\
Module##name::ComponentRegistrator::ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)()) {\
        Module##name::components.insert(std::make_pair(name, fn));\
      }\
std::map<std::string, std::shared_ptr<IComponent>(*)()> Module##name::components;

#define DECLARE_MODULE_COMPONENT(module_name, component_name) \
  static Module##module_name::ComponentRegistrator reg; \
  static std::shared_ptr<IComponent> create() { \
    return std::make_shared< component_name >(); \
  }

#define REGISTER_MODULE_COMPONENT(module_name, component_name) \
  Module##module_name::ComponentRegistrator component_name::reg(#component_name, &component_name::create);
