#include "ModuleLoader.h"

#include <utility>

class LoadedModule {
public:
    explicit LoadedModule(std::shared_ptr<IModule> module);

    std::string name();

    std::vector<std::string> getComponentsList();

    bool hasComponent(const std::string &name);

    std::shared_ptr<IComponent> getComponent(const std::string &name);

    std::optional<ProvidedProfile> getProfile();
private:
    std::shared_ptr<IModule> m_module;
};

ModuleLoader::ModuleLoader(const std::string &modules_directory) {
    std::vector<boost::dll::fs::path> plugins = {};
    for (auto const &dir_entry : std::filesystem::directory_iterator{ modules_directory })
        plugins.emplace_back(dir_entry.path().string());

    for (const auto &plugin_path : plugins) {
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

ModuleLoader::~ModuleLoader() {
    m_modules.clear();
    m_libs.clear();
}

std::vector<std::string> ModuleLoader::getModulesList() {
    std::vector<std::string> modules;

    for (const auto &mod : m_modules)
        modules.push_back(mod.first);

    return modules;
}

std::vector<std::string> ModuleLoader::getModuleComponentsList(const std::string &module) {
    auto it = m_modules.find(module);
    if (it != m_modules.end())
        return it->second->getComponentsList();

    return {};
}

std::size_t ModuleLoader::count() const {
    return m_modules.size();
}

bool ModuleLoader::hasModuleComponent(const std::string &module_name, const std::string &component_name) {
    if (m_modules.count(module_name)) {
        return m_modules[module_name]->hasComponent(component_name);
    }
    return false;
}

std::shared_ptr<IComponent> ModuleLoader::getModuleComponent(const std::string &module_name, const std::string &component_name) {
    if (m_modules.count(module_name)) {
        return m_modules[module_name]->getComponent(component_name);
    }
    return {};
}

std::optional<ProvidedProfile> ModuleLoader::getModuleProfile(const std::string &module_name) {
    return m_modules[module_name]->getProfile();
}

LoadedModule::LoadedModule(std::shared_ptr<IModule> module)
    : m_module(std::move(module)) {}

std::string LoadedModule::name() {
    return m_module->getName();
}

std::vector<std::string> LoadedModule::getComponentsList() {
    return m_module->getComponentList();
}

bool LoadedModule::hasComponent(const std::string &name) {
    return m_module->hasComponent(name);
}

std::shared_ptr<IComponent> LoadedModule::getComponent(const std::string &name) {
    return m_module->createComponent(name);
}

std::optional<ProvidedProfile> LoadedModule::getProfile() {
    return m_module->getProvidedProfile();
}