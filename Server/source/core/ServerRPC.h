#pragma once

#include <rpc/server.h>

#include "DeviceController.h"
#include "ModuleLoader.h"

MSGPACK_ADD_ENUM(image::helper::EImageFormat);

class ServerRPC
{
public:
    ServerRPC(uint16_t port, std::shared_ptr<DeviceController> dev_ctrl, std::shared_ptr<ModuleLoader> mod_ctrl);

    int start();
private:
    std::shared_ptr<DeviceController> m_device_controller;
    std::shared_ptr<ModuleLoader> m_module_loader;

    rpc::server m_server;
};