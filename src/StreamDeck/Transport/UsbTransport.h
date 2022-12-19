#pragma once

#include "Transport.h"

#include <hidapi/hidapi.h>

#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <mutex>
#include <shared_mutex>


struct UsbDeviceInfo
{
    UsbDeviceInfo(std::string path, unsigned short vendor_id, unsigned short product_id)
        : path(path), vendor_id(vendor_id), product_id(product_id) {}

    std::string path;
    unsigned short vendor_id;
    unsigned short product_id;
};

class LibUSBHIDWrapper
{
public:
    LibUSBHIDWrapper()
    {
        if (hid_init() != 0)
        {
            throw std::runtime_error("hid_init failed");
        }
    };

    std::vector<UsbDeviceInfo> enumerate(unsigned short vendor_id = 0, unsigned short product_id = 0)
    {
        std::vector<UsbDeviceInfo> devices;

        std::unique_lock lock(hid_mutex);
        hid_device_info* device_info_enum = hid_enumerate(vendor_id, product_id);
        // lock.unlock(); if hid_enumerate returns independent list
        hid_device_info* device_info = device_info_enum;
        while (device_info)
        {
            devices.emplace_back(device_info->path, device_info->vendor_id, device_info->product_id);
            device_info = device_info->next;
        }
        hid_free_enumeration(device_info);

        return devices;
    };

    hid_device* open_device(const std::string &path)
    {
        std::lock_guard lock(hid_mutex);
        hid_device* device = hid_open_path(path.c_str());

        if (!device)
            throw std::runtime_error("Could not open HID device.");

        if (hid_set_nonblocking(device, 1) != 0)
            throw std::runtime_error("Could not set device non blocking.");

        return device;
    }

    void close_device(hid_device* device)
    {
        if (device)
        {
            std::lock_guard lock(hid_mutex);
            hid_close(device);
        }
    }

    int send_feature_report(hid_device* device, const unsigned char *data, size_t size)
    {
        if (!device)
            throw std::runtime_error("No HID device (send_feature_report).");

        std::unique_lock lock(hid_mutex);
        int result = hid_send_feature_report(device, data, size);
        lock.unlock();

        if (result < 0)
            throw std::runtime_error("Failed to write feature report: " + std::to_string(result));

        return result;
    }

    std::vector<unsigned char> get_feature_report(hid_device* device, unsigned char report_id, size_t length)
    {
        if (!device)
            throw std::runtime_error("No HID device (read).");

        std::vector<unsigned char> data(length);
        data[0] = report_id;

        std::unique_lock lock(hid_mutex);
        int result = hid_get_feature_report(device, &*data.begin(), length);
        lock.unlock();

        if (result < 0)
            throw std::runtime_error("Failed to read feature report: " + std::to_string(result));

        return data;
    }

    int write(hid_device* device, unsigned char *data, size_t size)
    {
        std::unique_lock lock(hid_mutex);
        if (!device)
            throw std::runtime_error("No HID device.");

        int result = hid_write(device, data, size);

        if (result < 0)
            throw std::runtime_error("Failed to write out report: " + std::to_string(result));

        return result;
    }

    std::vector<unsigned char> read(hid_device* device, size_t length)
    {
        std::vector<unsigned char> data(length);

        if (!device)
            throw std::runtime_error("No HID device.");

        std::unique_lock lock(hid_mutex);
        int result = hid_read(device, data.data(), length);
        lock.unlock();

        if (result < 0)
            throw std::runtime_error("Failed to read report: " + std::to_string(result));

        if (result == 0)
            return {};

        return data;
    }

private:
    std::shared_mutex hid_mutex; // why shared?
};


class UsbDevice : public IDevice
{
public:
    UsbDevice(std::shared_ptr<LibUSBHIDWrapper> libUsb, UsbDeviceInfo device_info)
        : m_libUsb(libUsb), m_device_info(device_info) {}

    ~UsbDevice() override
    {
        close();
    }

    void open() override
    {
        std::lock_guard lock(device_mutex);

        if (m_device_handle)
            return;

        m_device_handle = m_libUsb->open_device(m_device_info.path);
    }

    void close() override
    {
        std::unique_lock lock(device_mutex);

        if (!m_device_handle)
            return;

        m_libUsb->close_device(m_device_handle);
        m_device_handle = nullptr;
    }

    bool is_open() const override
    {
        std::lock_guard lock(device_mutex);
        return m_device_handle != nullptr;
    }

    bool connected() const override
    {
        std::unique_lock lock(device_mutex);
        const auto devices = m_libUsb->enumerate();
        lock.unlock();

        const auto device_path = m_device_info.path;
        auto it = std::find_if(begin(devices), end(devices),
                               [&device_path](const auto &dev) { return dev.path == device_path; });
        return it != std::end(devices);
    }

    unsigned short vendor_id() const override
    {
        return m_device_info.vendor_id;
    }

    unsigned short product_id() const override
    {
        return m_device_info.product_id;
    }

    std::string path() const override
    {
        return m_device_info.path;
    }

    int write_feature(std::vector<unsigned char> payload) override
    {
        std::lock_guard lock(device_mutex);
        return m_libUsb->send_feature_report(m_device_handle, payload.data(), payload.size());
    }

    std::vector<unsigned char> read_feature(unsigned char report_id, size_t length) override
    {
        std::lock_guard lock(device_mutex);
        return m_libUsb->get_feature_report(m_device_handle, report_id, length);
    }

    int write(std::vector<unsigned char> data) override
    {
        std::lock_guard lock(device_mutex);
        return m_libUsb->write(m_device_handle, data.data(), data.size());
    }

    std::vector<unsigned char> read(size_t length) override
    {
        std::lock_guard lock(device_mutex);
        return m_libUsb->read(m_device_handle, length);
    }

private:
    std::shared_ptr<LibUSBHIDWrapper> m_libUsb;
    UsbDeviceInfo m_device_info;
    hid_device* m_device_handle = nullptr;

    mutable std::shared_mutex device_mutex;
};

class UsbTransport : public ITransport
{
public:
    std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid) override
    {
        auto lib_usb = std::make_shared<LibUSBHIDWrapper>();
        std::vector<std::shared_ptr<IDevice>> ret_devices;
        const auto devices = lib_usb->enumerate(vid, pid);
        for (auto device : devices)
        {
            ret_devices.emplace_back(std::make_shared<UsbDevice>(lib_usb, device));
        }
        return ret_devices;
    }
};
