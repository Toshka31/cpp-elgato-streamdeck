#include <util/LibUsbHidWrapper.h>

LibUSBHIDWrapper::LibUSBHIDWrapper()
{
    if (hid_init() != 0)
    {
        throw std::runtime_error("hid_init failed");
    }
};

std::vector<UsbDeviceInfo> LibUSBHIDWrapper::enumerate(unsigned short vendor_id, unsigned short product_id)
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

hid_device* LibUSBHIDWrapper::open_device(const std::string &path)
{
    std::lock_guard lock(hid_mutex);
    hid_device* device = hid_open_path(path.c_str());

    if (!device)
        throw std::runtime_error("Could not open HID device.");

    if (hid_set_nonblocking(device, 1) != 0)
        throw std::runtime_error("Could not set device non blocking.");

    return device;
}

void LibUSBHIDWrapper::close_device(hid_device* device)
{
    if (device)
    {
        std::lock_guard lock(hid_mutex);
        hid_close(device);
    }
}

int LibUSBHIDWrapper::send_feature_report(hid_device* device, const unsigned char *data, size_t size)
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

std::vector<unsigned char> LibUSBHIDWrapper::get_feature_report(hid_device* device, unsigned char report_id, size_t length)
{
    if (!device)
        throw std::runtime_error("No HID device (read).");

    std::vector<unsigned char> data(length);
    data[0] = report_id;

    std::unique_lock lock(hid_mutex);
    int result = hid_get_feature_report(device, data.data(), length);
    lock.unlock();

    if (result < 0)
        throw std::runtime_error("Failed to read feature report: " + std::to_string(result));

    return data;
}

int LibUSBHIDWrapper::write(hid_device* device, unsigned char *data, size_t size)
{
    if (!device)
        throw std::runtime_error("No HID device (write).");

    std::unique_lock lock(hid_mutex);
    int result = hid_write(device, data, size);
    lock.unlock();

    if (result < 0)
        throw std::runtime_error("Failed to write out report: " + std::to_string(result));

    return result;
}

std::vector<unsigned char> LibUSBHIDWrapper::read(hid_device* device, size_t length)
{
    if (!device)
        throw std::runtime_error("No HID device.");

    std::vector<unsigned char> data(length);

    std::unique_lock lock(hid_mutex);
    int result = hid_read(device, data.data(), length);
    lock.unlock();

    if (result < 0)
        throw std::runtime_error("Failed to read report: " + std::to_string(result));

    if (result == 0)
        return {};

    return data;
}