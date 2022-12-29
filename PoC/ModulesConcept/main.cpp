#include <IModule.h>

#include <boost/dll/import.hpp>

#include <iostream>
#include <map>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

std::vector<fs::path> get_all(fs::path const & root, std::string const & ext)
{
    std::vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root))
    {
        for (auto const & entry : fs::recursive_directory_iterator(root))
        {
            if (fs::is_regular_file(entry) && entry.path().extension() == ext)
                paths.emplace_back(entry.path().filename());
        }
    }

    return paths;
}     

class LoadedModule
{
public:
    LoadedModule(std::shared_ptr<ModuleCore> module) : m_module(module) 
    {
        m_components = module->getComponentList();
    }

    std::string name()
    {
        return m_module->getName();
    }

    std::vector<std::string> getComponentsList()
    {
        return m_components;
    }

    std::shared_ptr<IComponent> getComponent(const std::string &name)
    {
        return m_module->createComponent(name);
    }
private:
    std::shared_ptr<ModuleCore> m_module;
    std::vector<std::string> m_components;
};

class ModuleLoader 
{
public:
    ModuleLoader(const boost::dll::fs::path& modules_directory)
    {        
        std::vector<boost::dll::fs::path> plugins = get_all(modules_directory, ".so");
        
        for (std::size_t i = 0; i < plugins.size(); ++i) {
            boost::dll::fs::path shared_library_path(modules_directory);
            shared_library_path /= plugins[i];
            boost::dll::shared_library lib(shared_library_path, boost::dll::load_mode::append_decorations);
            if (!lib.has("create")) {
                continue;
            }

            typedef std::shared_ptr<ModuleCore> (pluginapi_create_t)();
            std::function<pluginapi_create_t> creator = boost::dll::import_alias<pluginapi_create_t>(boost::move(lib), "create");
            std::shared_ptr<ModuleCore> plugin = creator();

            m_libs.push_back(std::make_shared<boost::dll::shared_library>(shared_library_path)); // this is way to prevent unload

            m_modules[plugin->getName()] = std::make_shared<LoadedModule>(plugin);
        }
    }

    ~ModuleLoader()
    {
        m_modules.clear();
        m_libs.clear();
    }

    void printModules() const
    {
        for (const auto &mod : m_modules)
        {
            std::cout << mod.second->name() << std::endl;
            for (const auto &comp : mod.second->getComponentsList())
                std::cout << "\t " << comp << std::endl;
        }
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

int main()
{
    ModuleLoader module_loader("/home/anton/dev/experiments/build/lib/");

    std::cout << module_loader.count() << std::endl;
    module_loader.printModules();

    std::cout << "boo";
}