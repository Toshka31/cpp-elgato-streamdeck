#include "DeviceController.h"

#include <device/DeviceManager.h>
#include <device/TransportFactory.h>

DeviceController::DeviceController(std::shared_ptr<ModuleLoader> mod_loader, std::shared_ptr<ITransport> transport)
    : m_mod_loader(mod_loader),
      m_dev_manager(transport)
{}

void DeviceController::tick()
{
    std::lock_guard guard(m_mutex);
    for (const auto& device : m_registered_deices)
    {
        if (device.second->is_device_open())
        {
            device.second->tick();
        }
    }
}

void DeviceController::deviceInspector()
{
    auto streamdecks = m_dev_manager.enumerate();

    for (auto &deck : streamdecks)
    {
        deck->open();
        deck->reset();

        auto reg_device = std::make_shared<RegisteredDevice>(deck, m_mod_loader);
        reg_device->init();
        std::lock_guard guard(m_mutex);
        m_registered_deices.insert({deck->get_serial_number(), reg_device});
    }
}

std::vector<std::string> DeviceController::getDevicesList() const
{
    std::lock_guard guard(m_mutex);
    std::vector<std::string> devices;
    for (const auto& device : m_registered_deices)
        devices.push_back(device.first);
    return devices;
}

void DeviceController::setDeviceBrightness(const std::string &device_id, unsigned char brightness)
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setBrightness(brightness);
}

void DeviceController::setDeviceCurrentProfile(const std::string &device_id, const std::string &profile)
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setProfile(profile);
}

void DeviceController::setDeviceCurrentPage(const std::string &device_id, const std::string &page)
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setPage(page);
}

void DeviceController::setDeviceButtonImage(const std::string &device_id, unsigned char button, std::vector<unsigned char>& image)
{
    std::lock_guard guard(m_mutex);
    std::cout << "image.size() = " << image.size() << std::endl;
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setButtonImage(button, image);
}

void DeviceController::setDeviceButtonLabel(const std::string &device_id, unsigned char button, const std::string &label)
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setButtonLabel(button, label);
}

void DeviceController::setDeviceButtonComponent(const std::string &device_id, unsigned char button,
                                                const std::string &module, const std::string &component)
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end()) {
        it->second->setButtonComponent(button, module, component);
    }
}

int DeviceController::getDeviceBrightness(const std::string &device_id) const {
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getBrightness();
    return {};
}

std::string DeviceController::getDeviceCurrentProfile(const std::string &device_id) const
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getCurrentProfileName();
    return {};
}

std::vector<std::string> DeviceController::getDeviceProfiles(const std::string &device_id) const
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getProfiles();
    return {};
}

std::string DeviceController::getDeviceCurrentPage(const std::string &device_id) const
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getCurrentPageName();
    return {};
}

std::vector<std::string> DeviceController::getDevicePages(const std::string &device_id) const
{
    std::lock_guard guard(m_mutex);
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getPages();
    return {};
}
