#pragma once

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>

#include <boost/dll/import.hpp>

#include <map>
#include <iostream>
#include <filesystem>

class LoadedModule
{
public:
    LoadedModule(std::shared_ptr<IModule> module) : m_module(module)
    {

    }

    std::string name()
    {
        return m_module->getName();
    }

    std::vector<std::string> getComponentsList()
    {
        return m_module->getComponentList();
    }

    bool hasComponent(const std::string &name)
    {
        return m_module->hasComponent(name);
    }

    std::shared_ptr<IComponent> getComponent(const std::string &name)
    {
        return m_module->createComponent(name);
    }

    std::optional<ProvidedProfile> getProfile()
    {
        return m_module->getProvidedProfile();
    }
private:
    std::shared_ptr<IModule> m_module;
};

class ModuleLoader 
{
public:
    ModuleLoader(const std::string &modules_directory)
    {
        std::vector<boost::dll::fs::path> plugins = {};
        for (auto const& dir_entry : std::filesystem::directory_iterator{modules_directory})
          plugins.emplace_back(dir_entry.path().string());

        for (const auto & plugin_path : plugins) {
            try {
                boost::dll::shared_library lib(plugin_path, boost::dll::load_mode::append_decorations);
                if (!lib.has("create")) {
                    continue;
                }

                typedef std::shared_ptr<IModule> (pluginapi_create_t)();
                std::function<pluginapi_create_t>
                    creator = boost::dll::import_alias<pluginapi_create_t>(boost::move(lib), "create");
                std::shared_ptr<IModule> plugin = creator();

                m_libs
                    .push_back(std::make_shared<boost::dll::shared_library>(plugin_path)); // this is way to prevent unload

                m_modules[plugin->getName()] = std::make_shared<LoadedModule>(plugin);
            }
            catch (std::runtime_error &e) {
                std::cout << "Failed to load " + plugin_path.string() << " with error:" << e.what() << std::endl;
            }
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

    bool hasModuleComponent(const std::string &module_name, const std::string &component_name)
    {
        if (m_modules.count(module_name)) {
            return m_modules[module_name]->hasComponent(component_name);
        }
        return false;
    }

    std::shared_ptr<IComponent> getModuleComponent(const std::string &module_name, const std::string &component_name)
    {
        if (m_modules.count(module_name))
        {
            return m_modules[module_name]->getComponent(component_name);
        }
        return {};
    }

    std::optional<ProvidedProfile> getModuleProfile(const std::string &module_name)
    {
        return m_modules[module_name]->getProfile();
    }
private:
    boost::dll::fs::path plugins_directory_;
    // Name => Module
    std::map<std::string, std::shared_ptr<LoadedModule>> m_modules;
    std::vector<std::shared_ptr<boost::dll::shared_library>> m_libs;
    
};
