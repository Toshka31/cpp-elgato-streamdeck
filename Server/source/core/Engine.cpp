#include "Engine.h"

#include "DeviceController.h"
#include "device/TransportFactory.h"

#include <3rdparty/nlohmann/json.hpp>

#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>

Engine::Engine()
{
    std::filesystem::path homedir = system_util::getHomeDirectory();

    std::filesystem::path modules_path = homedir / FOLDER_STREAMDECK / FOLDER_MODULES;
    if (!std::filesystem::exists(modules_path))
        std::filesystem::create_directories(modules_path);
    m_module_loader = std::make_shared<ModuleLoader>(modules_path.string());

#ifdef EMULATOR
    auto transport = TransportFactory::createDebugTransport();
#else
    auto transport = TransportFactory::createUsbTransport();
#endif
    m_device_controller = std::make_shared<DeviceController>(m_module_loader, transport);

    m_rpc_server = std::make_shared<ServerRPC>(27015, m_device_controller, m_module_loader);
}

[[noreturn]] int Engine::start()
{
    std::thread thread_rpc(&ServerRPC::start, m_rpc_server);
    std::thread thread_dev_inspector(&DeviceController::deviceInspector, m_device_controller);

    m_is_running = true;

    while ( m_is_running )
    {
        m_device_controller->tick();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
        std::this_thread::yield();
    }

    if (thread_rpc.joinable())
        thread_rpc.join();
    if (thread_dev_inspector.joinable())
        thread_dev_inspector.join();
}
