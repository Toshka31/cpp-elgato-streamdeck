#include "ModuleLoader.h"
#include "Profile.h"
#include "RegisteredDevice.h"
#include "ConfigLoader.h"

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

class Engine
{
public:
    Engine()
    {
        struct passwd *pw = getpwuid(getuid());
        std::filesystem::path homedir(pw->pw_dir);

        std::filesystem::path modules_path = homedir / FOLDER_STREAMDECK / FOLDER_MODULES;
        m_module_loader = std::make_shared<ModuleLoader>(modules_path.string());

        DeviceManager mngr(TransportFactory::createUsbTransport());
	    auto streamdecks = mngr.enumerate();

        for (auto deck : streamdecks)
        {
            deck->open();
            deck->reset();

            std::string deck_serial = deck->get_serial_number();
            std::filesystem::path configs_path = homedir / FOLDER_STREAMDECK;
            Profile profile = loadDeckConfiguration(deck_serial, configs_path);
            auto reg_device = std::make_shared<RegisteredDevice>(deck, profile);
            reg_device->init(m_module_loader);
            m_registered_deices.insert({deck_serial, reg_device});
        }
    }

    void start()
    {
        rpc::server srv(11925);

        srv.bind("testCall", [this](){ return testCall(); });

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

    int testCall()
    {
        return 10;
    }
protected:
    std::shared_ptr<ModuleLoader> m_module_loader;
    std::map<std::string, std::shared_ptr<RegisteredDevice>> m_registered_deices;
};