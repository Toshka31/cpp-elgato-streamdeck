#include "StreamDeckClient.h"

#include <rpc/client.h>

MSGPACK_ADD_ENUM(image::helper::EImageFormat);

StreamDeckClient::StreamDeckClient(const std::string &addr, uint16_t port)
    : m_rpc_client(addr, port)
{
}

std::map<std::string, std::vector<std::string>> StreamDeckClient::getComponentsList() {
    return m_rpc_client.call("getComponentsList").as<std::map<std::string, std::vector<std::string>>>();
}

std::vector<std::string> StreamDeckClient::getDevicesList() {
    return m_rpc_client.call("getDevicesList").as<std::vector<std::string>>();
}

std::string StreamDeckClient::getDeviceCurrentProfile(const std::string &device_id)
{
    return std::string();
}

std::vector<std::string> StreamDeckClient::getDeviceProfiles(const std::string &device_id) {
    return m_rpc_client.call("getDeviceProfiles", device_id).as<std::vector<std::string>>();
}

std::string StreamDeckClient::getDeviceCurrentPage(const std::string &device_id)
{
    return std::string();
}

std::vector<std::string> StreamDeckClient::getDevicePages(const std::string &device_id) {
    return m_rpc_client.call("getDevicePages", device_id).as<std::vector<std::string>>();
}

void StreamDeckClient::setDeviceBrightness(const std::string &device_id, unsigned char brightness) {

}

void StreamDeckClient::setDeviceButtonImage(const std::string &device_id, unsigned char button,
                                            const std::vector<uint8_t> &image, image::helper::EImageFormat format) {

}

void StreamDeckClient::setDeviceButtonComponent(const std::string &device_id, unsigned char button,
                                                const std::string &module, const std::string &component) {

}
