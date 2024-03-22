#include "UsbDevice.h"

#include <algorithm>
#include <utility>

UsbDevice::UsbDevice(std::shared_ptr<LibUSBHIDWrapper> libUsb, UsbDeviceInfo device_info)
    : m_libUsb(std::move(libUsb)), m_device_info(std::move(device_info))
{}

UsbDevice::~UsbDevice() {
    close(); // TODO avoid call virtual function from destructor
}

void UsbDevice::open() {
    std::lock_guard lock(device_mutex);

    if (m_device_handle)
        return;

    m_device_handle = m_libUsb->open_device(m_device_info.path);
}

void UsbDevice::close() {
    std::unique_lock lock(device_mutex);

    if (!m_device_handle)
        return;

    m_libUsb->close_device(m_device_handle);
    m_device_handle = nullptr;
}

bool UsbDevice::is_open() const {
    std::lock_guard lock(device_mutex);
    return m_device_handle != nullptr;
}

bool UsbDevice::connected() const {
    std::unique_lock lock(device_mutex);
    const auto devices = m_libUsb->enumerate();
    lock.unlock();

    const auto device_path = m_device_info.path;
    auto it = std::find_if(begin(devices), end(devices),
                           [&device_path](const auto &dev) { return dev.path == device_path; }
    );
    return it != std::end(devices);
}

unsigned short UsbDevice::vendor_id() const {
    return m_device_info.vendor_id;
}

unsigned short UsbDevice::product_id() const {
    return m_device_info.product_id;
}

std::string UsbDevice::path() const {
    return m_device_info.path;
}

int UsbDevice::write_feature(std::vector<unsigned char> payload) {
    std::lock_guard lock(device_mutex);
    return m_libUsb->send_feature_report(m_device_handle, payload.data(), payload.size());
}

std::vector<unsigned char> UsbDevice::read_feature(unsigned char report_id, size_t length) {
    std::lock_guard lock(device_mutex);
    return m_libUsb->get_feature_report(m_device_handle, report_id, length);
}

int UsbDevice::write(std::vector<unsigned char> data) {
    std::lock_guard lock(device_mutex);
    return m_libUsb->write(m_device_handle, data.data(), data.size());
}

std::vector<unsigned char> UsbDevice::read(size_t length) {
    std::lock_guard lock(device_mutex);
    return m_libUsb->read(m_device_handle, length);
}