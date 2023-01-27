#include "Profile.h"

#include "3rdparty/nlohmann/json.hpp"

#include <filesystem>
#include <fstream>

Profile::Profile(const std::filesystem::path &path)
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
        for (auto &json_key : json_page.at("keys"))
        {
            Key key;
            ushort key_number;
            json_data.at("number").get_to(key_number);
            json_data.at("image").get_to(key.m_custom_image);
            json_data.at("module").get_to(key.m_module_name);
            json_data.at("component").get_to(key.m_component_name);
            page.m_keys.insert({key_number, key});
        }
    }
}

void Profile::save()
{
    nlohmann::json json_data;

    json_data["brightness"] = m_brightness;
    json_data["page"] = m_current_page->m_name;
    nlohmann::json arr_pages = nlohmann::json::array();
    for (const auto &page : m_pages)
    {
        nlohmann::json json_page = nlohmann::json::object();
        json_page["name"] = page.first;
        nlohmann::json arr_keys = nlohmann::json::array();
        for (const auto &key : page.second->m_keys)
        {
            nlohmann::json json_key = nlohmann::json::object();
            json_key["number"] = key.first;
            json_key["image"]  = key.second.m_custom_image;
            json_key["module"] = key.second.m_module_name;
            json_key["component"] = key.second.m_component_name;
        }
        json_page["keys"] = arr_keys;

        json_page.push_back(json_page);
    }
    json_data["pages"] = arr_pages;
}

void Profile::setBrightness(const ushort value)
{
    m_brightness = value;
}

void Profile::setButtonImage(ushort button, const std::string &cached_image_path)
{
    m_current_page->m_keys[button] = {cached_image_path};
}

void Profile::setButtonComponent(ushort button, const std::string &module_name, const std::string &component_name)
{
    m_current_page->m_keys[button] = {module_name, component_name};
}

std::vector<std::string> Profile::getPages() const
{
    std::vector<std::string> result;
    for (const auto &page : m_pages)
        result.push_back(page.first);
    return result;
}
