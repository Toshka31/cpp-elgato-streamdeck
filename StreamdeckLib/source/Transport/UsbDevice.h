#pragma once

#include <StreamDeckLib/Transport/IDevice.h>
#include "LibUsbHidWrapper.h"

#include <memory>
#include <vector>
#include <string>
#include <mutex>


class UsbDevice : public IDevice
{
public:
    UsbDevice(std::shared_ptr<LibUSBHIDWrapper> libUsb, UsbDeviceInfo device_info);

    ~UsbDevice() override;

    void open() override;

    void close() override;

    bool is_open() const override;

    bool connected() const override;

    unsigned short vendor_id() const override;

    unsigned short product_id() const override;

    std::string path() const override;

    int write_feature(std::vector<unsigned char> payload) override;

    std::vector<unsigned char> read_feature(unsigned char report_id, size_t length) override;

    int write(std::vector<unsigned char> data) override;

    std::vector<unsigned char> read(size_t length) override;

private:
    std::shared_ptr<LibUSBHIDWrapper> m_libUsb;
    UsbDeviceInfo m_device_info;
    hid_device* m_device_handle = nullptr;

    mutable std::mutex device_mutex;
};