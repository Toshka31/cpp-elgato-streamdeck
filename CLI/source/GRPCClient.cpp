
#include "GRPCClient.h"

std::map<std::string, std::vector<std::string>> StreamDeckClient::getComponents() {
    StreamDeckAPI::ModuleListResponse reply;

    grpc::ClientContext context;
    grpc::Status status = stub_->GetComponents(&context, google::protobuf::Empty(), &reply);

    if (status.ok() && !reply.response().has_error()) {
        std::map<std::string, std::vector<std::string>> result;
        for (const auto &module : reply.modules()) {
            std::vector<std::string> components(module.components().begin(), module.components().end());
            result[module.name()] = components;
        }
        return result;
    } else {
        std::cout << "Error: " << status.error_code() << ": " << status.error_message() << std::endl;
        return {};
    }
}

std::vector<std::string> StreamDeckClient::getDevices() {
    grpc::ClientContext context;
    google::protobuf::Empty request;
    StreamDeckAPI::DeviceListResponse response;

    grpc::Status status = stub_->GetDevices(&context, request, &response);
    if (status.ok()) {
        std::vector<std::string> result;
        result.reserve(response.devices().size());
        std::copy(response.devices().begin(), response.devices().end(), result.begin());
        return result;
    }
    return {};
}

std::vector<std::string> StreamDeckClient::getDeviceProfiles(const std::string &device) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceProfileListResponse response;
    StreamDeckAPI::DeviceRequest request;
    request.set_device_name(device);

    grpc::Status status = stub_->GetDeviceProfiles(&context, request, &response);
    if (status.ok()) {
        std::vector<std::string> result;
        result.reserve(response.profiles().size());
        std::copy(response.profiles().begin(), response.profiles().end(), result.begin());
        return result;
    }
    return {};
}

std::string StreamDeckClient::getDeviceCurrentProfile(const std::string &device) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceCurrentProfileResponse response;
    StreamDeckAPI::DeviceRequest request;
    request.set_device_name(device);

    grpc::Status status = stub_->GetDeviceCurrentProfile(&context, request, &response);
    if (status.ok()) {
        std::vector<std::string> result;
        return response.profile();
    }
    return {};
}

int32_t StreamDeckClient::setDeviceCurrentProfile(const std::string &device, const std::string &profile) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetProfileRequest request;
    request.set_device(device);
    request.set_profile(profile);

    grpc::Status status = stub_->SetDeviceCurrentProfile(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}

std::vector<std::string> StreamDeckClient::getDevicePages(const std::string &device) {
    grpc::ClientContext context;
    StreamDeckAPI::DevicePageListResponse response;
    StreamDeckAPI::DeviceRequest request;
    request.set_device_name(device);

    grpc::Status status = stub_->GetDevicePages(&context, request, &response);
    if (status.ok()) {
        std::vector<std::string> result;
        result.reserve(response.pages().size());
        std::copy(response.pages().begin(), response.pages().end(), result.begin());
        return result;
    }
    return {};
}

std::string StreamDeckClient::getDeviceCurrentPage(const std::string &device) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceCurrentPageResponse response;
    StreamDeckAPI::DeviceRequest request;
    request.set_device_name(device);

    grpc::Status status = stub_->GetDeviceCurrentPage(&context, request, &response);
    if (status.ok()) {
        std::vector<std::string> result;
        return response.page();
    }
    return {};
}

int32_t StreamDeckClient::setDeviceCurrentPage(const std::string &device, const std::string &page) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetPageRequest request;
    request.set_device(device);
    request.set_page(page);

    grpc::Status status = stub_->SetDeviceCurrentPage(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}

std::vector<char> StreamDeckClient::getDeviceButtonImage(const std::string &device, int button) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceButtonImageResponse response;
    StreamDeckAPI::DeviceButtonRequest request;
    request.set_device_name(device);
    request.set_button(button);

    grpc::Status status = stub_->GetDeviceButtonImage(&context, request, &response);
    if (status.ok()) {
        std::vector<char> result(response.image().begin(), response.image().end());
        return result;
    }
    return {};
}

std::string StreamDeckClient::getDeviceButtonLabel(const std::string &device, int button) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceButtonLabelResponse response;
    StreamDeckAPI::DeviceButtonRequest request;
    request.set_device_name(device);
    request.set_button(button);

    grpc::Status status = stub_->GetDeviceButtonLabel(&context, request, &response);
    if (status.ok()) {
        return response.label();
    }
    return {};
}

std::pair<std::string, std::string> StreamDeckClient::getDeviceButtonModuleComponent(const std::string &device, int button) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceButtonComponentResponse response;
    StreamDeckAPI::DeviceButtonRequest request;
    request.set_device_name(device);
    request.set_button(button);

    grpc::Status status = stub_->GetDeviceButtonModuleComponent(&context, request, &response);
    if (status.ok()) {
        return { response.module(), response.component() };
    }
    return {};
}

int32_t StreamDeckClient::getDeviceBrightness(const std::string &device) {
    grpc::ClientContext context;
    StreamDeckAPI::DeviceBrightnessResponse response;
    StreamDeckAPI::DeviceRequest request;
    request.set_device_name(device);

    grpc::Status status = stub_->GetDeviceBrightness(&context, request, &response);
    if (status.ok()) {
        return response.brightness();
    }
    return -1;
}

int32_t StreamDeckClient::setDeviceBrightness(const std::string &device, int32_t brightness) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetBrightnessRequest request;
    request.set_device(device);
    request.set_brightness(brightness);

    grpc::Status status = stub_->SetDeviceBrightness(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}

int32_t StreamDeckClient::setDeviceButtonImage(const std::string &device, int button, const std::vector<char> &img_data) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetButtonImageRequest request;
    request.set_device(device);
    request.set_button(button);
    std::string data(img_data.begin(), img_data.end());
    request.set_image(data);

    grpc::Status status = stub_->SetDeviceButtonImage(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}

int32_t StreamDeckClient::setDeviceButtonLabel(const std::string &device, int button, const std::string &label) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetButtonLabelRequest request;
    request.set_device(device);
    request.set_button(button);
    request.set_label(label);

    grpc::Status status = stub_->SetDeviceButtonLabel(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}

int32_t StreamDeckClient::setDeviceButtonModuleComponent(
    const std::string &device,
    int button,
    const std::string &module,
    const std::string &component
) {
    grpc::ClientContext context;
    StreamDeckAPI::Response response;
    StreamDeckAPI::DeviceSetButtonModuleComponentRequest request;
    request.set_device(device);
    request.set_button(button);
    request.set_module(module);
    request.set_component(component);

    grpc::Status status = stub_->SetDeviceButtonModuleComponent(&context, request, &response);
    if (status.ok()) {
        return response.code();
    }
    return -1;
}
