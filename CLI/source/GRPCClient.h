#ifndef ELGATO_STREAMDECK_GRPCCLIENT_H
#define ELGATO_STREAMDECK_GRPCCLIENT_H

#include <boost/program_options.hpp>

#include <grpcpp/grpcpp.h>
#include <StreamDeck/API/api.grpc.pb.h>

class StreamDeckClient {
public:
    StreamDeckClient(std::shared_ptr<grpc::Channel> channel)
            : stub_(StreamDeckAPI::StreamDeckAPI::NewStub(channel)) {}

    std::map<std::string, std::vector<std::string>> getComponents();

    std::vector<std::string> getDevices();

    std::vector<std::string> getDeviceProfiles(const std::string &device);

    std::string getDeviceCurrentProfile(const std::string &device);

    int32_t setDeviceCurrentProfile(const std::string &device, const std::string &profile );

    std::vector<std::string> getDevicePages(const std::string &device);

    std::string getDeviceCurrentPage(const std::string &device);

    int32_t setDeviceCurrentPage(const std::string &device, const std::string &page );

    std::vector<char> getDeviceButtonImage(const std::string &device, int button);

    std::string getDeviceButtonLabel(const std::string &device, int button);

    std::pair<std::string, std::string> getDeviceButtonModuleComponent(const std::string &device, int button);

    int32_t getDeviceBrightness(const std::string &device);

    int32_t setDeviceBrightness(const std::string &device, int32_t brightness);

    int32_t setDeviceButtonImage(const std::string &device, int button, const std::vector<char> &img_data);

    int32_t setDeviceButtonLabel(const std::string &device, int button, const std::string &label);

    int32_t setDeviceButtonModuleComponent(const std::string &device, int button, const std::string &module, const std::string &component);

private:
    std::unique_ptr<StreamDeckAPI::StreamDeckAPI::Stub> stub_;
};


#endif //ELGATO_STREAMDECK_GRPCCLIENT_H
