
#pragma once

#include "ModuleLoader.h"
#include "RegisteredDevice.h"

class Engine
{
public:
    Engine();

    [[noreturn]] void start();

private:
    std::map<std::string, std::vector<std::string>> getComponentsList() const;

    std::vector<std::string> getDevicesList() const;

    void setDeviceBrightness(const std::string &device_id, unsigned char brightness);

    void setDeviceButtonImage(const std::string &device_id, unsigned char button, std::vector<unsigned char>& image);

    void setDeviceButtonLabel(const std::string &device_id, unsigned char button, const std::string &label);

    void setDeviceButtonComponent(const std::string &device_id, unsigned char button, const std::string &module, const std::string &component);

    std::string getDeviceCurrentProfile(const std::string &device_id) const;

    std::vector<std::string> getDeviceProfiles(const std::string &device_id) const;

    std::string getDeviceCurrentPage(const std::string &device_id) const;

    std::vector<std::string> getDevicePages(const std::string &device_id) const;


protected:
    std::shared_ptr<ModuleLoader> m_module_loader;
    std::map<std::string, std::shared_ptr<RegisteredDevice>> m_registered_deices;
};
