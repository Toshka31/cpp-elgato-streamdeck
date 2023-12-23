#pragma once

#include <hidapi/hidapi.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <mutex>


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
    LibUSBHIDWrapper();

    std::vector<UsbDeviceInfo> enumerate(unsigned short vendor_id = 0, unsigned short product_id = 0);

    hid_device* open_device(const std::string &path);

    void close_device(hid_device* device);

    int send_feature_report(hid_device* device, const unsigned char *data, size_t size);

    std::vector<unsigned char> get_feature_report(hid_device* device, unsigned char report_id, size_t length);

    int write(hid_device* device, unsigned char *data, size_t size);

    std::vector<unsigned char> read(hid_device* device, size_t length);

private:
    std::mutex hid_mutex;
};
