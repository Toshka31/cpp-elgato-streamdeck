#include "Engine.h"

#include "DeviceController.h"

#include <3rdparty/nlohmann/json.hpp>

#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>

Engine::Engine()
{
    std::filesystem::path homedir = system_util::getHomeDirectory();

    std::filesystem::path modules_path = homedir / FOLDER_STREAMDECK / FOLDER_MODULES;
    m_module_loader = std::make_shared<ModuleLoader>(modules_path.string());

    m_device_controller = std::make_shared<DeviceController>(m_module_loader);

    m_rpc_server = std::make_shared<ServerRPC>(27015, m_device_controller, m_module_loader);
}

[[noreturn]] int Engine::start()
{
    // TODO thread for RPC server
    // TODO thread for Devices
    for ( ;; ) // TODO variable to proper terminating
    {
        m_device_controller->tick();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        std::this_thread::yield();
    }
}
