#include "DeviceController.h"

#include <device/DeviceManager.h>
#include <device/TransportFactory.h>

DeviceController::DeviceController(std::shared_ptr<ModuleLoader> mod_loader) {
#ifdef EMULATOR
    DeviceManager mngr(TransportFactory::createDebugTransport());
#else
    DeviceManager mngr(TransportFactory::createUsbTransport());
#endif
    auto streamdecks = mngr.enumerate();

    // TODO make device inspecting in separate function (and possible thread)
    for (auto &deck : streamdecks)
    {
        deck->open();
        deck->reset();

        auto reg_device = std::make_shared<RegisteredDevice>(deck, mod_loader);
        reg_device->init();
        m_registered_deices.insert({deck->get_serial_number(), reg_device});
    }
}

void DeviceController::tick() {
    for (const auto& device : m_registered_deices)
    {
        if (device.second->is_device_open())
        {
            device.second->tick();
        }
    }
}

std::vector<std::string> DeviceController::getDevicesList() const
{
    std::vector<std::string> devices;
    for (const auto& device : m_registered_deices)
        devices.push_back(device.first);
    return devices;
}

void DeviceController::setDeviceBrightness(const std::string &device_id, unsigned char brightness)
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setBrightness(brightness);
}

void DeviceController::setDeviceButtonImage(const std::string &device_id, unsigned char button, std::vector<unsigned char>& image)
{
    std::cout << "image.size() = " << image.size() << std::endl;
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setButtonImage(button, image);
}

void DeviceController::setDeviceButtonLabel(const std::string &device_id, unsigned char button, const std::string &label)
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        it->second->setButtonLabel(button, label);
}

void DeviceController::setDeviceButtonComponent(const std::string &device_id, unsigned char button,
                                                const std::string &module, const std::string &component)
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end()) {
        it->second->setButtonComponent(button, module, component);
    }
}

std::string DeviceController::getDeviceCurrentProfile(const std::string &device_id) const
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getCurrentProfileName();
    return {};
}

std::vector<std::string> DeviceController::getDeviceProfiles(const std::string &device_id) const
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getProfiles();
    return {};
}

std::string DeviceController::getDeviceCurrentPage(const std::string &device_id) const
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getCurrentPageName();
    return {};
}

std::vector<std::string> DeviceController::getDevicePages(const std::string &device_id) const
{
    auto it = m_registered_deices.find(device_id);
    if (it != m_registered_deices.end())
        return it->second->getPages();
    return {};
}
