#include <grpcpp/grpcpp.h>
#include <StreamDeck/API/api.grpc.pb.h>

#include "DeviceController.h"
#include "ModuleLoader.h"

class ServerGRPC final : public StreamDeckAPI::StreamDeckAPI::Service
{
public:
    ServerGRPC(uint16_t port, std::shared_ptr<DeviceController> dev_ctrl, std::shared_ptr<ModuleLoader> mod_ctrl);

    int start();

    grpc::Status GetComponents(grpc::ServerContext* context,
                               const google::protobuf::Empty* request,
                               StreamDeckAPI::ModuleListResponse* response) final;

    grpc::Status GetDevices(grpc::ServerContext* context,
                            const google::protobuf::Empty* request,
                            StreamDeckAPI::DeviceListResponse* response) final;

    grpc::Status GetDeviceProfiles(grpc::ServerContext* context,
                                   const StreamDeckAPI::DeviceRequest* request,
                                   StreamDeckAPI::DeviceProfileListResponse* response) final;

    grpc::Status GetDeviceCurrentProfile(grpc::ServerContext* context,
                                         const StreamDeckAPI::DeviceRequest* request,
                                         StreamDeckAPI::DeviceCurrentProfileResponse* response) final;

    grpc::Status SetDeviceCurrentProfile(grpc::ServerContext* context,
                                         const ::StreamDeckAPI::DeviceSetProfileRequest* request,
                                         StreamDeckAPI::Response* response) final;

    grpc::Status GetDevicePages(grpc::ServerContext* context,
                                const StreamDeckAPI::DeviceRequest* request,
                                StreamDeckAPI::DevicePageListResponse* response) final;

    grpc::Status GetDeviceCurrentPage(grpc::ServerContext* context,
                                      const StreamDeckAPI::DeviceRequest* request,
                                      StreamDeckAPI::DeviceCurrentPageResponse* response) final;

    grpc::Status SetDeviceCurrentPage(grpc::ServerContext* context,
                                      const StreamDeckAPI::DeviceSetPageRequest* request,
                                      StreamDeckAPI::Response* response) final;

    grpc::Status GetDeviceBrightness(grpc::ServerContext* context,
                                     const StreamDeckAPI::DeviceRequest* request,
                                     StreamDeckAPI::DeviceBrightnessResponse* response) final;

    grpc::Status SetDeviceBrightness(grpc::ServerContext* context,
                                     const StreamDeckAPI::DeviceSetBrightnessRequest* request,
                                     StreamDeckAPI::Response* response) final;

    grpc::Status SetDeviceButtonImage(grpc::ServerContext* context,
                                      const StreamDeckAPI::DeviceSetButtonImageRequest* request,
                                      StreamDeckAPI::Response* response) final;

    grpc::Status SetDeviceButtonLabel(grpc::ServerContext* context,
                                      const StreamDeckAPI::DeviceSetButtonLabelRequest* request,
                                      StreamDeckAPI::Response* response) final;

    grpc::Status SetDeviceButtonModuleComponent(grpc::ServerContext* context,
                                                const ::StreamDeckAPI::DeviceSetButtonModuleComponentRequest* request,
                                                StreamDeckAPI::Response* response) final;
private:
    std::string m_address;

    std::shared_ptr<DeviceController> m_device_controller;
    std::shared_ptr<ModuleLoader> m_module_loader;


};