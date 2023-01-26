#include "ModuleLoader.h"
#include "RegisteredDevice.h"
#include "ConfigLoader.h"
#include "util.h"

#include <StreamDeckLib/DeviceManager.h>
#include <StreamDeckLib/Transport/TransportFactory.h>

#include <3rdparty/nlohmann/json.hpp>

#include <chrono>
#include <thread>
#include <mutex>
#include <filesystem>
#include <fstream>

#include <rpc/server.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

MSGPACK_ADD_ENUM(image::helper::EImageFormat);

class Engine
{
public:
    Engine()
    {
        std::filesystem::path homedir = util::getHomeDirectory();

        std::filesystem::path modules_path = homedir / FOLDER_STREAMDECK / FOLDER_MODULES;
        m_module_loader = std::make_shared<ModuleLoader>(modules_path.string());

        DeviceManager mngr(TransportFactory::createUsbTransport());
	    auto streamdecks = mngr.enumerate();

        for (auto &deck : streamdecks)
        {
            deck->open();
            deck->reset();

            auto reg_device = std::make_shared<RegisteredDevice>(deck, m_module_loader);
            m_registered_deices.insert({deck->get_serial_number(), reg_device});
        }
    }

    void start()
    {
        rpc::server srv(11925);

        srv.bind(
            "getComponentsList", 
            [this]() -> std::map<std::string, std::vector<std::string>> { 
                return getComponentsList();
            }
        );
        srv.bind(
            "getDevicesList", 
            [this]() -> std::vector<std::string> { 
                return getDevicesList();
            }
        );
        srv.bind(
            "setDeviceBrightness", 
            [this](const std::string &device_id, unsigned char brightness) -> void {
                setDeviceBrightness(device_id, brightness);
            }
        );
        srv.bind(
            "setDeviceButtonImage", 
            [this](const std::string &device_id, unsigned char button, std::vector<uint8_t> image, image::helper::EImageFormat format) -> void { 
                std::cout << "image.size() = " << image.size() << std::endl;
                setDeviceButtonImage(device_id, button, image, format);
            }
        );
        srv.bind(
            "setDeviceButtonComponent", 
            [this](const std::string &device_id, unsigned char button, const std::string &module, const std::string &component) -> void { 
                setDeviceButtonComponent(device_id, button, module, component);
            }
        );

        srv.async_run();

        for ( ;; )
        {
            for (auto device : m_registered_deices)
            {
                if (device.second->is_device_open())
                {
                    device.second->tick();
                }
            }

            std::this_thread::sleep_for(100ms);
            std::this_thread::yield();
        }
    }

    std::map<std::string, std::vector<std::string>> getComponentsList()
    {
        std::map<std::string, std::vector<std::string>> ret;

        std::vector<std::string> modules = m_module_loader->getModulesList();
        for (const auto &module_name : modules)
            ret[module_name] = m_module_loader->getModuleComponentsList(module_name);

        return ret;
    }

    std::vector<std::string> getDevicesList()
    {
        std::vector<std::string> devices;
        for (const auto device : m_registered_deices)
            devices.push_back(device.first);
        return devices;
    }

    void setDeviceBrightness(const std::string &device_id, unsigned char brightness)
    {
        auto it = m_registered_deices.find(device_id);
        if (it != m_registered_deices.end())
            it->second->setBrightness(brightness);
    }

    void setDeviceButtonImage(const std::string &device_id, unsigned char button, std::vector<unsigned char> image, image::helper::EImageFormat format)
    {
        std::cout << "image.size() = " << image.size() << std::endl;
        auto it = m_registered_deices.find(device_id);
        if (it != m_registered_deices.end())
            it->second->setButtonImage(button, image, format);
    }

    void setDeviceButtonComponent(const std::string &device_id, unsigned char button, const std::string &module, const std::string &component)
    {
        auto it = m_registered_deices.find(device_id);
        if (it != m_registered_deices.end())
            it->second->setButtonComponent(button, module, component);
    }

    std::vector<std::string> getDeviceProfiles(const std::string &device_id)
    {
        auto it = m_registered_deices.find(device_id);
        if (it != m_registered_deices.end())
            return it->second->getProfiles();
        return {};
    }

    std::vector<std::string> getDevicePages(const std::string &device_id)
    {
        auto it = m_registered_deices.find(device_id);
        if (it != m_registered_deices.end())
            return it->second->getPages();
        return {};
    }

protected:
    std::shared_ptr<ModuleLoader> m_module_loader;
    std::map<std::string, std::shared_ptr<RegisteredDevice>> m_registered_deices;
};
