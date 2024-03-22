#include "ServerRPC.h"

#include <utility>

ServerRPC::ServerRPC(uint16_t port, std::shared_ptr<DeviceController> dev_ctrl, std::shared_ptr<ModuleLoader> mod_ctrl)
    : m_device_controller(std::move(dev_ctrl)), m_module_loader(std::move(mod_ctrl)), m_server(port)
{
    m_server.bind(
        "getComponentsList",
        [this]() -> std::map<std::string, std::vector<std::string>> {
            std::map<std::string, std::vector<std::string>> ret;

            std::vector<std::string> modules = m_module_loader->getModulesList();
            for (const auto &module_name : modules)
                ret[module_name] = m_module_loader->getModuleComponentsList(module_name);

            return ret;
        }
    );
    m_server.bind(
        "getDevicesList",
        [this]() -> std::vector<std::string> {
            return m_device_controller->getDevicesList();
        }
    );
    m_server.bind(
        "getDeviceProfiles",
        [this](const std::string &device_id) -> std::vector<std::string> {
            return m_device_controller->getDeviceProfiles(device_id);
        }
    );
    m_server.bind(
        "getDevicePages",
        [this](const std::string &device_id) -> std::vector<std::string> {
            return m_device_controller->getDevicePages(device_id);
        }
    );
    m_server.bind(
        "getDeviceCurrentPage",
        [this](const std::string &device_id) -> std::string {
            return m_device_controller->getDeviceCurrentPage(device_id);
        }
    );
    m_server.bind(
        "getDeviceCurrentProfile",
        [this](const std::string &device_id) -> std::string {
            return m_device_controller->getDeviceCurrentProfile(device_id);
        }
    );
    m_server.bind(
        "setDeviceBrightness",
        [this](const std::string &device_id, unsigned char brightness) -> void {
            m_device_controller->setDeviceBrightness(device_id, brightness);
        }
    );
    m_server.bind(
        "setDeviceButtonImage",
        [this](const std::string &device_id, unsigned char button, std::vector<uint8_t> &image) -> void {
            m_device_controller->setDeviceButtonImage(device_id, button, image);
        }
    );
    m_server.bind(
        "setDeviceButtonLabel",
        [this](const std::string &device_id, unsigned char button, std::string &label) -> void {
            m_device_controller->setDeviceButtonLabel(device_id, button, label);
        }
    );
    m_server.bind(
        "setDeviceButtonComponent",
        [this](const std::string &device_id, unsigned char button, const std::string &module, const std::string &component) -> void {
            m_device_controller->setDeviceButtonComponent(device_id, button, module, component);
        }
    );

}

int ServerRPC::start() {
    m_server.run();
}
