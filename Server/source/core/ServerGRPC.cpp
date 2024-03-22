#include "ServerGRPC.h"

#include <utility>

ServerGRPC::ServerGRPC(
    uint16_t port,
    std::shared_ptr<DeviceController> dev_ctrl,
    std::shared_ptr<ModuleLoader> mod_ctrl
)
    : m_device_controller(std::move(dev_ctrl))
    , m_module_loader(std::move(mod_ctrl))
{
    m_address = "0.0.0.0:" + std::to_string(port);
}

int ServerGRPC::start() {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(m_address, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << m_address << std::endl;
    server->Wait();
}

grpc::Status ServerGRPC::GetComponents(
    grpc::ServerContext *context,
    const google::protobuf::Empty *request,
    StreamDeckAPI::ModuleListResponse *response
) {
    std::vector<std::string> modules = m_module_loader->getModulesList();
    for (const auto &module_name : modules) {
        auto *mod = response->add_modules();
        mod->set_name(module_name);
        auto components = m_module_loader->getModuleComponentsList(module_name);
        for (const auto &component : components) {
            mod->add_components(component);
        }
    }
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::GetDevices(
    grpc::ServerContext *context,
    const google::protobuf::Empty *request,
    StreamDeckAPI::DeviceListResponse *response
) {
    auto devices = m_device_controller->getDevicesList();
    for (const auto &device : devices)
        response->add_devices(device);
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::GetDeviceProfiles(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceRequest *request,
    StreamDeckAPI::DeviceProfileListResponse *response
) {
    auto profiles = m_device_controller->getDeviceProfiles(request->device_name());
    for (const auto &profile : profiles)
        response->add_profiles(profile);
    return grpc::Status::OK;
}

grpc::Status
ServerGRPC::GetDeviceCurrentProfile(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceRequest *request,
    StreamDeckAPI::DeviceCurrentProfileResponse *response
) {
    auto current_profile = m_device_controller->getDeviceCurrentProfile(request->device_name());
    response->set_profile(current_profile);
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceCurrentProfile(
    grpc::ServerContext *context,
    const ::StreamDeckAPI::DeviceSetProfileRequest *request,
    StreamDeckAPI::Response *response
) {
    m_device_controller->setDeviceCurrentProfile(request->device(), request->profile());
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::GetDevicePages(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceRequest *request,
    StreamDeckAPI::DevicePageListResponse *response
) {
    auto pages = m_device_controller->getDevicePages(request->device_name());
    for (const auto &page : pages)
        response->add_pages(page);
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::GetDeviceCurrentPage(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceRequest *request,
    StreamDeckAPI::DeviceCurrentPageResponse *response
) {
    auto current_page = m_device_controller->getDeviceCurrentPage(request->device_name());
    response->set_page(current_page);
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceCurrentPage(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceSetPageRequest *request,
    StreamDeckAPI::Response *response
) {
    m_device_controller->setDeviceCurrentPage(request->device(), request->page());
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::GetDeviceBrightness(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceRequest *request,
    StreamDeckAPI::DeviceBrightnessResponse *response
) {
    m_device_controller->getDeviceBrightness(request->device_name());
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceBrightness(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceSetBrightnessRequest *request,
    StreamDeckAPI::Response *response
) {
    m_device_controller->setDeviceBrightness(request->device(), request->brightness());
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceButtonImage(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceSetButtonImageRequest *request,
    StreamDeckAPI::Response *response
) {
    std::vector<unsigned char> img_data(request->image().begin(), request->image().end());
    m_device_controller->setDeviceButtonImage(request->device(), request->button(), img_data);
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceButtonLabel(
    grpc::ServerContext *context,
    const StreamDeckAPI::DeviceSetButtonLabelRequest *request,
    StreamDeckAPI::Response *response
) {
    m_device_controller->setDeviceButtonLabel(request->device(), request->button(), request->label());
    return grpc::Status::OK;
}

grpc::Status ServerGRPC::SetDeviceButtonModuleComponent(
    grpc::ServerContext *context,
    const ::StreamDeckAPI::DeviceSetButtonModuleComponentRequest *request,
    StreamDeckAPI::Response *response
) {
    m_device_controller->setDeviceButtonComponent(request->device(), request->button(), request->module(), request->component());
    return grpc::Status::OK;
}
