#pragma once

#include <ModuleAPI/IModule.h>
#include <ModuleAPI/IComponent.h>

#include <boost/dll/import.hpp>

#include <map>
#include <vector>
#include <iostream>
#include <filesystem>

class LoadedModule;

class ModuleLoader
{
public:
    ModuleLoader(const std::string &modules_directory);

    ~ModuleLoader();

    std::vector<std::string> getModulesList();

    std::vector<std::string> getModuleComponentsList(const std::string &module);

    std::size_t count() const;

    bool hasModuleComponent(const std::string &module_name, const std::string &component_name);

    std::shared_ptr<IComponent> getModuleComponent(const std::string &module_name, const std::string &component_name);

    std::optional<ProvidedProfile> getModuleProfile(const std::string &module_name);

private:
    boost::dll::fs::path plugins_directory_;
    // Name => Module
    std::map<std::string, std::shared_ptr<LoadedModule>> m_modules;
    std::vector<std::shared_ptr<boost::dll::shared_library>> m_libs;
    
};
