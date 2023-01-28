#include <StreamDeckLib/ImageHelper/ImageHelper.h>

#include <iostream>
#include <fstream>
#include <rpc/client.h>

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
    
        for (auto dev : result)
            std::cout << dev << std::endl;
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

            if (buffer.size())
                client.call("setDeviceButtonImage", argv[2], atoi(argv[3]), buffer);
        }
    }
    else if (!strcmp(argv[1], "set_component"))
    {
        if (argc != 6)
            return 0;

        client.call("setDeviceButtonComponent", argv[2], atoi(argv[3]), argv[4], argv[5]);
    }
}