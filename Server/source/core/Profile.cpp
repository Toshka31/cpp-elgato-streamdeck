#include "Profile.h"

#include "3rdparty/nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <utility>

KeyProfile KeyProfile::createKeyProfileWithImage(std::string custom_image) {
    KeyProfile key_profile;
    key_profile.m_custom_image = std::move(custom_image);
    return key_profile;
}

KeyProfile KeyProfile::createKeyProfileWithComponent(std::string module_name, std::string component_name) {
    KeyProfile key_profile;
    key_profile.m_module_name = std::move(module_name);
    key_profile.m_component_name = std::move(component_name);
    return key_profile;
}

KeyProfile KeyProfile::createKeyProfileWithLabel(std::string label) {
    KeyProfile key_profile;
    key_profile.m_custom_label = std::move(label);
    return key_profile;
}

Profile::Profile(const std::filesystem::path &path)
    : m_path(path) {
    m_profile_name = path.stem().string();

    std::ifstream f(path);
    nlohmann::json json_data = nlohmann::json::parse(f);

    json_data.at("brightness").get_to(m_brightness);
    std::string current_page_name;
    json_data.at("page").get_to(current_page_name);
    for (auto json_page : json_data.at("pages")) {
        auto page = std::make_shared<Page>();
        json_page.at("name").get_to(page->m_name);
        for (auto &json_key : json_page.at("keys")) {
            KeyProfile key;
            ushort key_number;
            json_key.at("number").get_to(key_number);
            json_key.at("image").get_to(key.m_custom_image);
            json_key.at("label").get_to(key.m_custom_label);
            json_key.at("module").get_to(key.m_module_name);
            json_key.at("component").get_to(key.m_component_name);
            page->m_keys.insert({ key_number, key });
        }
        m_pages.insert({ page->m_name, page });
        if (page->m_name == current_page_name)
            m_current_page = page;
    }
}

void Profile::save() {
    nlohmann::json json_data;

    json_data["brightness"] = m_brightness;
    json_data["page"] = m_current_page->m_name;
    nlohmann::json arr_pages = nlohmann::json::array();
    for (const auto &page : m_pages) {
        nlohmann::json json_page = nlohmann::json::object();
        json_page["name"] = page.first;
        nlohmann::json arr_keys = nlohmann::json::array();
        for (const auto &key : page.second->m_keys) {
            nlohmann::json json_key = nlohmann::json::object();
            json_key["number"] = key.first;
            json_key["image"] = key.second.m_custom_image;
            json_key["label"] = key.second.m_custom_label;
            json_key["module"] = key.second.m_module_name;
            json_key["component"] = key.second.m_component_name;
            arr_keys.push_back(json_key);
        }
        json_page["keys"] = arr_keys;

        arr_pages.push_back(json_page);
    }
    json_data["pages"] = arr_pages;

    std::ofstream file(m_path);
    file << json_data;
}

void Profile::setBrightness(const ushort value) {
    m_brightness = value;
    save();
}

void Profile::setButtonImage(ushort button, const std::string &cached_image_path) {
    const auto it = m_current_page->m_keys.find(button);
    if (it != m_current_page->m_keys.end())
        it->second.m_custom_image = cached_image_path;
    else
        m_current_page->m_keys[button] = KeyProfile::createKeyProfileWithImage(cached_image_path);
    save();
}

void Profile::setButtonLabel(ushort button, const std::string &label) {
    const auto it = m_current_page->m_keys.find(button);
    if (it != m_current_page->m_keys.end())
        it->second.m_custom_label = label;
    else
        m_current_page->m_keys[button] = KeyProfile::createKeyProfileWithLabel(label);
    save();
}

void Profile::setButtonComponent(ushort button, const std::string &module_name, const std::string &component_name) {
    const auto it = m_current_page->m_keys.find(button);
    if (it != m_current_page->m_keys.end()) {
        it->second.m_module_name = module_name;
        it->second.m_component_name = component_name;
    } else
        m_current_page->m_keys[button] = KeyProfile::createKeyProfileWithComponent(module_name, component_name);
    save();
}

void Profile::setPage(const std::string &page) {
    const auto it = m_pages.find(page);
    if (it != m_pages.cend())
        m_current_page = it->second;
}

std::optional<KeyProfile> Profile::getCurrentKeyProfile(ushort key) {
    const auto it = m_current_page->m_keys.find(key);
    if (it != m_current_page->m_keys.end())
        return it->second;
    return {};
}

std::vector<std::string> Profile::getPages() const {
    std::vector<std::string> result;
    for (const auto &page : m_pages)
        result.push_back(page.first);
    return result;
}

std::string Profile::getName() const {
    return m_profile_name;
}

std::string Profile::getCurrentPageName() const {
    return m_current_page->m_name;
}
