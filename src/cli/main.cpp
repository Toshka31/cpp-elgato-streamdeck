#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <fstream>
#include <rpc/client.h>

#include "ModuleAPI/ComponentParameters.h"

// getComponentsList -> std::map<std::string, std::vector<std::string>>
// getDevicesList -> std::vector<std::string>
// setDeviceBrightness .. const std::string &device_id, unsigned char brightness
// setDeviceButtonImage .. const std::string &device_id, unsigned char button, std::vector<uint8_t> image, image::helper::EImageFormat format
// setDeviceButtonComponent .. const std::string &device_id, unsigned char button, const std::string &module, const std::string &component

MSGPACK_ADD_ENUM(image::helper::EImageFormat);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 0;
    }

    rpc::client client("127.0.0.1", 11925);

    if (!strcmp(argv[1], "components"))
    {
        auto result = client.call("getComponentsList").as<std::map<std::string, std::vector<std::string>>>();
    
        for (auto module : result)
        {
            std::cout << module.first << std::endl;
            for(auto comp : module.second)
                std::cout << "\t" << comp << std::endl;
        }
    }
    else if (!strcmp(argv[1], "devices"))
    {
        auto result = client.call("getDevicesList").as<std::vector<std::string>>();
    
        for (const auto& dev : result)
            std::cout << dev << std::endl;
    }
    else if (!strcmp(argv[1], "get_profiles"))
    {
        auto result = client.call("getDeviceProfiles", argv[2]).as<std::vector<std::string>>();

        for (const auto& prof : result)
            std::cout << prof << std::endl;
    }
    else if (!strcmp(argv[1], "get_pages"))
    {
        auto result = client.call("getDevicePages", argv[2]).as<std::vector<std::string>>();

        for (const auto& page : result)
            std::cout << page << std::endl;
    }
    else if (!strcmp(argv[1], "page"))
    {
        auto result = client.call("getDeviceCurrentPage", argv[2]).as<std::string>();

        std::cout << result << std::endl;
    }
    else if (!strcmp(argv[1], "profile"))
    {
        auto result = client.call("getDeviceCurrentProfile", argv[2]).as<std::string>();

        std::cout << result << std::endl;
    }
    else if (!strcmp(argv[1], "set_brightness"))
    {
        if (argc != 4)
            return 0;

        client.call("setDeviceBrightness", argv[2], atoi(argv[3]));
    }
    else if (!strcmp(argv[1], "set_image"))
    {
        std::cout << "set" << std::endl;

        std::string filename(argv[4]);

        std::ifstream file(argv[4], std::ios::binary | std::ios::ate);
        if (file)
        {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            file.read(buffer.data(), size);
            std::cout << "size = " << size << std::endl;
            std::cout << "image.size() = " << buffer.size() << std::endl;

            if (!buffer.empty())
                client.call("setDeviceButtonImage", argv[2], atoi(argv[3]), buffer);
        }
    }
    else if (!strcmp(argv[1], "set_label"))
    {
        std::string label = argv[4];
        client.call("setDeviceButtonLabel", argv[2], atoi(argv[3]), label);
    }
    else if (!strcmp(argv[1], "set_component"))
    {
        if (argc != 6)
            return 0;

        client.call("setDeviceButtonComponent", argv[2], atoi(argv[3]), argv[4], argv[5]);
    }
    else if (!strcmp(argv[1], "parameters"))
    {
        auto str = client.call("getDeviceButtonParameters", argv[2], atoi(argv[3])).as<std::string>();
        ComponentParameters params = ComponentParameters::deserialiseParameters(str);
        for (const auto &param : params.params)
        {
            switch (param.second->type) {
                case ComponentParameters::Parameter::INT:
                    std::cout << "(INT) " << param.first << " = " << std::get<int>(param.second->value) << std::endl;
                    break;
                case ComponentParameters::Parameter::DOUBLE:
                    std::cout << "(DOUBLE) " << param.first << " = " << std::get<double>(param.second->value) << std::endl;
                    break;
                case ComponentParameters::Parameter::STRING:
                    std::cout << "(STRING) " << param.first << " = " << std::get<std::string>(param.second->value) << std::endl;
                    break;
                case ComponentParameters::Parameter::LIST:
                    break;
            }
        }
    }
    else if (!strcmp(argv[1], "set_parameters"))
    {
        std::string device = argv[2];
        int button = atoi(argv[3]);

        ComponentParameters::Variables vars;

        int arg_index = 4;
        while (arg_index < argc)
        {
            if (argc - arg_index < 2)
                break;

            std::string var_name = argv[arg_index++];
            std::string var_type = argv[arg_index++];
            ComponentParameters::Value val;
            if (var_type == "INT") {
                val = atoi(argv[arg_index++]);
            }
            else if (var_type == "DOUBLE") {
                val = atof(argv[arg_index++]);
            }
            else if (var_type == "STRING") {
                val = argv[arg_index++];
            }

            vars.emplace_back(var_name, val);
        }

        client.call("setDeviceButtonParameters", device, button, ComponentParameters::serialiseVariables(vars));
    }
}