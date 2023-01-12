#pragma once

#include <ModuleAPI/IModule.h>

#include <boost/dll/import.hpp>

#include <map>
#include <iostream>

class LoadedModule
{
public:
    LoadedModule(std::shared_ptr<IModule> module) : m_module(module) 
    {
        for (auto component : module->getComponents())
            m_components[component->name()] = component;
    }

    std::string name()
    {
        return m_module->name();
    }

    std::vector<std::string> getComponentsList()
    {
        std::vector<std::string> res;
        for (auto comp : m_components)
            res.push_back(comp.first);
        return res;
    }

    std::shared_ptr<IComponent> getComponent(const std::string &name)
    {
        return m_components[name];
    }
private:
    std::shared_ptr<IModule> m_module;
    std::map<std::string, std::shared_ptr<IComponent>> m_components;
};

class ModuleLoader 
{
public:
    ModuleLoader(const boost::dll::fs::path& modules_directory)
    {        
        std::vector<boost::dll::fs::path> plugins = {};
        // TODO collect plugins from directory
        
        for (std::size_t i = 0; i < plugins.size(); ++i) {
            boost::dll::fs::path shared_library_path(modules_directory);
            shared_library_path /= plugins[i];
            boost::dll::shared_library lib(shared_library_path, boost::dll::load_mode::append_decorations);
            if (!lib.has("create")) {
                continue;
            }

            typedef std::shared_ptr<IModule> (pluginapi_create_t)();
            std::function<pluginapi_create_t> creator = boost::dll::import_alias<pluginapi_create_t>(boost::move(lib), "create");
            std::shared_ptr<IModule> plugin = creator();

            m_libs.push_back(std::make_shared<boost::dll::shared_library>(shared_library_path)); // this is way to prevent unload

            m_modules[plugin->name()] = std::make_shared<LoadedModule>(plugin);
        }
    }

    ~ModuleLoader()
    {
        m_modules.clear();
        m_libs.clear();
    }

    std::vector<std::string> getModulesList()
    {
        std::vector<std::string> modules;

        for (const auto &mod : m_modules)
            modules.push_back(mod.first);

        return modules;
    }

    std::vector<std::string> getModuleComponentsList(const std::string &module)
    {
        auto it = m_modules.find(module);
        if (it != m_modules.end())
            return it->second->getComponentsList();

        return {};
    }

    std::size_t count() const
    {
        return m_modules.size();
    }

    std::shared_ptr<IComponent> getModuleComponent(const std::string &module_name, const std::string &component_name)
    {
        return m_modules[module_name]->getComponent(component_name);
    }
private:
    boost::dll::fs::path plugins_directory_;
    // Name => Module
    std::map<std::string, std::shared_ptr<LoadedModule>> m_modules;
    std::vector<std::shared_ptr<boost::dll::shared_library>> m_libs;
    
};