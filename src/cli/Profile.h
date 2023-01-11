#pragma once

#include <3rdparty/nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

class Profile
{
public:
    Profile(const std::filesystem::path &path)
    {
        m_profile_name = path.stem().string();

        std::ifstream f(path);
        nlohmann::json json_data = nlohmann::json::parse(f);

        json_data.at("brightness").get_to(m_brightness);
        std::string current_page_name;
        json_data.at("page").get_to(current_page_name);
        for (auto json_page : json_data.at("pages"))
        {
            Page page;
            std::string page_name;
            json_page.at("name").get_to(page_name);
            for (auto json_key : json_page.at("keys"))
            {
                Key key;
                ushort key_number;
                json_data.at("number").get_to(key_number);
                json_data.at("module").get_to(key.m_module_name);
                json_data.at("component").get_to(key.m_component_name);
                page.m_keys.insert({key_number, key});
            }
        }
    }
    void save()
    {

    }

private:
    std::string m_profile_name;
    ushort m_brightness;

    struct Key {
        std::string m_module_name;
        std::string m_component_name;
    };
    struct Page {
        std::map<ushort, Key> m_keys;
    };
    std::map<std::string, std::shared_ptr<Page>> m_pages;
    std::shared_ptr<Page> m_current_page;
};