#ifndef ELGATO_STREAMDECK_STREAMDECKCLIENT_H
#define ELGATO_STREAMDECK_STREAMDECKCLIENT_H

#include <rpc/client.h>

#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <string>
#include <vector>
#include <map>

class StreamDeckClient {
public:
    explicit StreamDeckClient(std::string const &addr = "127.0.0.1", uint16_t port = 11925);

    std::map<std::string, std::vector<std::string>> getComponentsList();

    std::vector<std::string> getDevicesList();

    std::string getDeviceCurrentProfile(const std::string &device_id);

    std::vector<std::string> getDeviceProfiles(const std::string &device_id);

    std::string getDeviceCurrentPage(const std::string &device_id);

    std::vector<std::string> getDevicePages(const std::string &device_id);

    void setDeviceBrightness(const std::string &device_id, unsigned char brightness);

    void setDeviceButtonImage(const std::string &device_id, unsigned char button, const std::vector<uint8_t>& image, image::helper::EImageFormat format);

    void setDeviceButtonComponent(const std::string &device_id, unsigned char button, const std::string &module, const std::string &component);
private:
    rpc::client m_rpc_client;
};

#endif //ELGATO_STREAMDECK_STREAMDECKCLIENT_H
