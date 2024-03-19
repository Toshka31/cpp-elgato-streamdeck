
#pragma once

#include "DeviceController.h"
#include "ServerGRPC.h"

#include <vector>
#include <map>
#include <string>

class Engine
{
public:
    Engine();

    [[noreturn]] int start();

protected:
    std::shared_ptr<ModuleLoader> m_module_loader;
    std::shared_ptr<ServerGRPC> m_rpc_server;
    std::shared_ptr<DeviceController> m_device_controller;

    std::atomic_bool m_is_running;
};
