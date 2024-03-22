#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <fstream>
#include <rpc/client.h>

#include "ArgParser.h"
#include "Mediator.h"
#include "GRPCClient.h"

MSGPACK_ADD_ENUM(image::helper::EImageFormat);

int main(int argc, char *argv[]) {
    /* get
     * |-module
     *   |-list
     * |-device [ID|list]
     *   |-brightness
     *   |-page
     *   | |-list
     *   | |-current
     *   |-profile
     *   | |-list
     *   | |-current
     *   |-button
     *     |-image
     *     |-label
     *     |-component
     * set
     * |- device [ID]
     *   |- brightness [value 0-100]
     *   |- page [page m_name]
     *   |- profile [profile m_name]
     *   |- button [button ID]
     *     |- image [path]
     *     |- label [label]
     *     |- component [module] [component]
     */

    StreamDeckClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    Mediator mediator(client);

    ArgParser parser;

    auto get_cmd = parser.createSubCommand("get");
    get_cmd->createSubCommand("module")->createSubCommand("list")
        ->addAction([&mediator](auto &&PH1) { mediator.getModulesList(std::forward<decltype(PH1)>(PH1)); });

    get_cmd->createSubCommand("devices")
        ->addAction([&mediator](auto &&PH1) { mediator.getDevices(std::forward<decltype(PH1)>(PH1)); });

    auto get_device_cmd = get_cmd->createSubCommand("device")->addParameter("device_id");

    get_device_cmd->createSubCommand("brightness")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceBrightness(std::forward<decltype(PH1)>(PH1)); });

    auto get_device_page_cmd = get_device_cmd->createSubCommand("page");
    get_device_page_cmd->createSubCommand("list")
        ->addAction([&mediator](auto &&PH1) { mediator.getDevicePages(std::forward<decltype(PH1)>(PH1)); });
    get_device_page_cmd->createSubCommand("current")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceCurrentPage(std::forward<decltype(PH1)>(PH1)); });

    auto get_device_profile_cmd = get_device_cmd->createSubCommand("profile");
    get_device_profile_cmd->createSubCommand("list")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceProfiles(std::forward<decltype(PH1)>(PH1)); });
    get_device_profile_cmd->createSubCommand("current")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceCurrentProfile(std::forward<decltype(PH1)>(PH1)); });

    auto get_device_button_cmd = get_device_cmd->createSubCommand("button")->addParameter("button_id");
    get_device_button_cmd->createSubCommand("image")
        ->addParameter("save_path")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceButtonImage(std::forward<decltype(PH1)>(PH1)); });
    get_device_button_cmd->createSubCommand("label")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceButtonLabel(std::forward<decltype(PH1)>(PH1)); });
    get_device_button_cmd->createSubCommand("component")
        ->addAction([&mediator](auto &&PH1) { mediator.getDeviceButtonComponent(std::forward<decltype(PH1)>(PH1)); });

    auto set_cmd = parser.createSubCommand("set");
    auto set_device_cmd = set_cmd->createSubCommand("device")->addParameter("device_id");

    set_device_cmd->createSubCommand("brightness")->addParameter("brightness")
        ->addAction([&mediator](auto &&PH1) { mediator.setDeviceBrightness(std::forward<decltype(PH1)>(PH1)); });

    set_device_cmd->createSubCommand("page")->addParameter("page_name")
        ->addAction([&mediator](auto &&PH1) { mediator.setDevicePage(std::forward<decltype(PH1)>(PH1)); });

    set_device_cmd->createSubCommand("profile")->addParameter("profile_name")
        ->addAction([&mediator](auto &&PH1) { mediator.setDeviceProfile(std::forward<decltype(PH1)>(PH1)); });

    auto set_device_button_cmd = set_device_cmd->createSubCommand("button")->addParameter("button_id");
    set_device_button_cmd->createSubCommand("image")->addParameter("image_path")
        ->addAction([&mediator](auto &&PH1) { mediator.setDeviceButtonImage(std::forward<decltype(PH1)>(PH1)); });
    set_device_button_cmd->createSubCommand("label")->addParameter("label")
        ->addAction([&mediator](auto &&PH1) { mediator.setDeviceButtonLabel(std::forward<decltype(PH1)>(PH1)); });
    set_device_button_cmd
        ->createSubCommand("component")
        ->addParameter("module_name")
        ->addParameter("component_name")
        ->addAction([&mediator](auto &&PH1) { mediator.setDeviceButtonComponent(std::forward<decltype(PH1)>(PH1)); });

    parser.parse(argc, argv);

    return 0;
}