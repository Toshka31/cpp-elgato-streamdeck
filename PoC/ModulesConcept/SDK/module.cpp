#include "IModule.h"

#include <boost/dll/alias.hpp> 

#include <memory>
#include <map>

namespace {
    static std::map<std::string, std::shared_ptr<IComponent>(*)()> gl_components;
}

ComponentRegistrator::ComponentRegistrator(const std::string &name, std::shared_ptr<IComponent>(*fn)())
{
    gl_components.insert(std::make_pair(name, fn));
}

std::string ModuleCore::getName() const
{
    return "Noname";
}

std::vector<std::string> ModuleCore::getComponentList()
{
    std::vector<std::string> res;
    for (const auto &comp : gl_components)
        res.push_back(comp.first);
    return res;
}

std::shared_ptr<IComponent> ModuleCore::createComponent(const std::string& name) 
{
    return gl_components[name]();
}