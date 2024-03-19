#pragma once

#include <map>
#include <string>
#include <vector>

#include "RegisteredDevice.h"
#include "ModuleLoader.h"
#include "device/DeviceManager.h"

class DeviceController {
public:
    DeviceController(std::shared_ptr<ModuleLoader> mod_loader, std::shared_ptr<ITransport> transport);

    void tick();

    void deviceInspector();

    std::vector<std::string> getDevicesList() const;

    void setDeviceBrightness(const std::string &device_id, unsigned char brightness);

    void setDeviceCurrentProfile(const std::string &device_id, const std::string &profile);

    void setDeviceCurrentPage(const std::string &device_id, const std::string &page);

    void setDeviceButtonImage(const std::string &device_id, unsigned char button, std::vector<unsigned char>& image);

    void setDeviceButtonLabel(const std::string &device_id, unsigned char button, const std::string &label);

    void setDeviceButtonComponent(const std::string &device_id, unsigned char button, const std::string &module, const std::string &component);

    int getDeviceBrightness(const std::string &device_id) const;

    std::string getDeviceCurrentProfile(const std::string &device_id) const;

    std::vector<std::string> getDeviceProfiles(const std::string &device_id) const;

    std::string getDeviceCurrentPage(const std::string &device_id) const;

    std::vector<std::string> getDevicePages(const std::string &device_id) const;

private:
    DeviceManager m_dev_manager;
    std::shared_ptr<ModuleLoader> m_mod_loader;

    mutable std::mutex m_mutex;
    std::map<std::string, std::shared_ptr<RegisteredDevice>> m_registered_deices;
};
