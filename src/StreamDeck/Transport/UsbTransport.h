#pragma once

#include "Transport.h"

#include <hidapi/hidapi.h>

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
        hid_init();
    };

    std::vector<UsbDeviceInfo> enumerate(unsigned short vendor_id = 0, unsigned short product_id = 0)
    {
        std::vector<UsbDeviceInfo> devices;

        std::unique_lock lock(hid_mutex);
        hid_device_info* device_info_enum = hid_enumerate(vendor_id, product_id);
        if (device_info_enum)
        {
            hid_device_info* cur_device = device_info_enum;
            while (cur_device)
            {
                devices.emplace_back(cur_device->path, cur_device->vendor_id, cur_device->product_id);
                cur_device = cur_device->next;
            }
        }
        hid_free_enumeration(device_info_enum);

        return devices;
    };

    hid_device* open_device(const std::string &path)
    {
        std::unique_lock lock(hid_mutex);
        hid_device* device = hid_open_path(path.c_str());

        if (!device)
            throw std::runtime_error("Could not open HID device.");

        if (hid_set_nonblocking(device, 1) != 0)
            throw std::runtime_error("Could not set device non blocking.");

        return device;
    }

    void close_device(hid_device* device)
    {
        std::unique_lock lock(hid_mutex);
        if (device)
            hid_close(device);
    }

    int send_feature_report(hid_device* device, const unsigned char *data, size_t size)
    {
        std::unique_lock lock(hid_mutex);
        if (!device)
            throw std::runtime_error("No HID device.");

        int result = hid_send_feature_report(device, data, size);
        if (result < 0)
            throw std::runtime_error("Failed to write feature report: " + std::to_string(result));

        return result;
    }

    std::vector<unsigned char> get_feature_report(hid_device* device, unsigned char report_id, size_t length)
    {
        std::unique_lock lock(hid_mutex);
        if (!device)
            throw std::runtime_error("No HID device.");

        std::vector<unsigned char> data(length);
        data[0] = report_id;
        int result = hid_get_feature_report(device, &*data.begin(), length);
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

        std::unique_lock lock(hid_mutex);
        if (!device)
            throw std::runtime_error("No HID device.");

        int result = hid_read(device, &*data.begin(), length);

        if (result < 0)
            throw std::runtime_error("Failed to read report: " + std::to_string(result));
        else if (result == 0)
            return std::vector<unsigned char>();

        return data;
    }
private:
    std::shared_mutex hid_mutex;
};

class UsbDevice : public IDevice
{
public:
    UsbDevice(std::shared_ptr<LibUSBHIDWrapper> libUsb, UsbDeviceInfo device_info) : m_libUsb(libUsb), m_device_info(device_info) {};

    virtual ~UsbDevice() 
    {
        close();
    }

    void open() override
    {
        std::unique_lock lock(device_mutex);

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

    bool is_open() override
    {
        std::unique_lock lock(device_mutex);
        return m_device_handle != nullptr;
    }

    bool connected() override
    {
        std::unique_lock lock(device_mutex);
        for (const auto device : m_libUsb->enumerate())
        {
            if (device.path == m_device_info.path)
                return true;
        }
        return false;
    }

    unsigned short vendor_id() override
    {
        return m_device_info.vendor_id;
    }

    unsigned short product_id() override
    {
        return m_device_info.product_id;
    }

    std::string path() override
    {
        return m_device_info.path;
    }

    int write_feature(std::vector<unsigned char> payload) override
    {
        std::unique_lock lock(device_mutex);
        return m_libUsb->send_feature_report(m_device_handle, &*payload.begin(), payload.size());
    }

    std::vector<unsigned char> read_feature(unsigned char report_id, size_t length) override
    {
        std::unique_lock lock(device_mutex);
        return m_libUsb->get_feature_report(m_device_handle, report_id, length);
    }

    int write(std::vector<unsigned char> data) override
    {
        std::unique_lock lock(device_mutex);
        return m_libUsb->write(m_device_handle, &*data.begin(), data.size());
    }

    std::vector<unsigned char> read(size_t length) override
    {
        std::unique_lock lock(device_mutex);
        return m_libUsb->read(m_device_handle, length);
    }
private:
    std::shared_ptr<LibUSBHIDWrapper> m_libUsb;

    std::shared_mutex device_mutex;

    UsbDeviceInfo m_device_info;
    hid_device* m_device_handle = nullptr;
};

class UsbTransport : public ITransport
{
public:
    std::vector<std::shared_ptr<IDevice>> enumerate(unsigned short vid, unsigned short pid)
    {
        std::shared_ptr<LibUSBHIDWrapper> libUsbPtr = std::make_shared<LibUSBHIDWrapper>();

        std::vector<std::shared_ptr<IDevice>> ret_devices;

        std::vector<UsbDeviceInfo> devices = libUsbPtr->enumerate(vid, pid);
        for (auto device : devices)
        {
            ret_devices.emplace_back(std::make_shared<UsbDevice>(libUsbPtr, device));
        }

        return ret_devices;
    }
};