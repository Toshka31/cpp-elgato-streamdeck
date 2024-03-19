#pragma once

#include "GRPCClient.h"

#include <iostream>
#include <map>

class Mediator {
public:
    explicit Mediator(StreamDeckClient &cli) : m_client(cli) {}

    void getModulesList(const std::map<std::string, std::string> &args) {
        std::cout << "getModulesList" << std::endl;
        auto result = m_client.getComponents();
        for (const auto& module : result)
        {
            std::cout << module.first << std::endl;
            for(const auto& comp : module.second)
                std::cout << "\t" << comp << std::endl;
        }
    }

    void getDevices(const std::map<std::string, std::string> &args) {
        std::cout << "getDevices" << std::endl;
        auto result = m_client.getDevices();
        for (const auto& device : result) {
            std::cout << device << std::endl;
        }
    }

    void getDeviceBrightness(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id")) {
            std::cout << "getDeviceBrightness " << args.at("device_id") << std::endl;
            auto result = m_client.getDeviceBrightness(args.at("device_id"));
            std::cout << result << std::endl;
        }
        else
            std::cout << "getDeviceBrightness error";
    }

    void getDevicePages(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id")) {
            std::cout << "getDevicePages " << args.at("device_id") << std::endl;
            auto result = m_client.getDevicePages(args.at("device_id"));
            for (const auto& page : result)
                std::cout << page << std::endl;
        }
        else
            std::cout << "getDevicePages error";
    }

    void getDeviceCurrentPage(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id")) {
            std::cout << "getDeviceCurrentPage " << args.at("device_id") << std::endl;
            auto result = m_client.getDeviceCurrentPage(args.at("device_id"));
            std::cout << result << std::endl;
        }
        else
            std::cout << "getDeviceCurrentPage error";
    }

    void getDeviceProfiles(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id")) {
            std::cout << "getDeviceProfiles " << args.at("device_id") << std::endl;
            auto result = m_client.getDeviceProfiles(args.at("device_id"));
            for (const auto& prof : result)
                std::cout << prof << std::endl;
        }
        else
            std::cout << "getDeviceProfiles error";
    }

    void getDeviceCurrentProfile(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id")) {
            std::cout << "getDeviceCurrentProfile " << args.at("device_id") << std::endl;
            auto result = m_client.getDeviceCurrentProfile(args.at("device_id"));
            std::cout << result << std::endl;
        }
        else
            std::cout << "getDeviceCurrentProfile error";
    }

    void getDeviceButtonImage(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("button_id") && args.contains("save_path")) {
            std::cout << "getDeviceButtonImage " << args.at("device_id") << " " << args.at("button_id") << std::endl;
            auto img = m_client.getDeviceButtonImage(args.at("device_id"), std::atoi(args.at("button_id").c_str()));
            std::ofstream file(args.at("save_path"), std::ios::binary);
            std::copy(img.cbegin(), img.cend(), std::ostream_iterator<char>(file));
        }
        else
            std::cout << "getDeviceButtonImage error";
    }

    void getDeviceButtonLabel(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("button_id")) {
            std::cout << "getDeviceButtonLabel " << args.at("device_id") << " " << args.at("button_id") << std::endl;
            auto label = m_client.getDeviceButtonLabel(args.at("device_id"), std::atoi(args.at("button_id").c_str()));
            std::cout << label << std::endl;
        }
        else
            std::cout << "getDeviceButtonLabel error";
    }

    void getDeviceButtonComponent(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("button_id")) {
            std::cout << "getDeviceButtonComponent " << args.at("device_id") << " " << args.at("button_id") << std::endl;
            auto moduleComponent = m_client.getDeviceButtonModuleComponent(args.at("device_id"), std::atoi(args.at("button_id").c_str()));
            std::cout << moduleComponent.first << "\t" << moduleComponent.second << std::endl;
        }
        else
            std::cout << "getDeviceButtonComponent error";
    }

    void setDeviceBrightness(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("brightness")) {
            std::cout << "setDeviceBrightness " << args.at("brightness") << std::endl;
            m_client.setDeviceBrightness(args.at("device_id"), std::atoi(args.at("brightness").c_str()));
        }
        else
            std::cout << "setDeviceBrightness error";
    }

    void setDevicePage(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("page_name")) {
            std::cout << "setDevicePage " << args.at("page_name") << std::endl;
            m_client.setDeviceCurrentPage(args.at("device_id"), args.at("page_name"));
        }
        else
            std::cout << "setDevicePage error";
    }

    void setDeviceProfile(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("profile_name")) {
            std::cout << "setDeviceProfile " << args.at("profile_name") << std::endl;
            m_client.setDeviceCurrentProfile(args.at("device_id"), args.at("profile_name"));
        }
        else
            std::cout << "setDeviceProfile error";
    }

    void setDeviceButtonImage(const std::map<std::string, std::string> &args) {
        if (args.contains("image_path") && args.contains("button_id") && args.contains("image_path")) {
            std::cout << "setDeviceButtonImage " << args.at("image_path") << std::endl;

            std::ifstream file(args.at("image_path"), std::ios::binary | std::ios::ate);
            if (file) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                std::vector<char> buffer(size);
                file.read(buffer.data(), size);
                std::cout << "size = " << size << std::endl;
                std::cout << "image.size() = " << buffer.size() << std::endl;

                if (!buffer.empty())
                    m_client.setDeviceButtonImage(args.at("device_id"), std::stoi(args.at("button_id")), buffer);
            }
        }
        else
            std::cout << "setDeviceButtonImage error";
    }

    void setDeviceButtonLabel(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("button_id") && args.contains("label")) {
            std::cout << "setDeviceButtonLabel " << args.at("label") << std::endl;
            m_client.setDeviceButtonLabel(args.at("device_id"), std::atoi(args.at("button_id").c_str()), args.at("label"));
        }
        else
            std::cout << "setDeviceButtonLabel error";
    }

    void setDeviceButtonComponent(const std::map<std::string, std::string> &args) {
        if (args.contains("device_id") && args.contains("button_id") && args.contains("module_name") && args.contains("component_name")) {
            std::cout << "setDeviceButtonComponent " << args.at("module_name") << " " << args.at("component_name") << std::endl;
            m_client.setDeviceButtonModuleComponent(args.at("device_id"), std::atoi(args.at("button_id").c_str()), args.at("module_name"), args.at("component_name"));
        }
        else
            std::cout << "setDeviceButtonComponent error";
    }
private:
    StreamDeckClient &m_client;
};