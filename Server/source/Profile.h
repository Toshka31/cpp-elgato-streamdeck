#pragma once

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <optional>


struct KeyProfile
{
    KeyProfile() = default;

    static KeyProfile createKeyProfileWithLabel(std::string label);
    static KeyProfile createKeyProfileWithImage(std::string custom_image);
    static KeyProfile createKeyProfileWithComponent(std::string module_name,
                                                    std::string component_name);
    std::string m_custom_image;
    std::string m_custom_label;
    std::string m_module_name;
    std::string m_component_name;
};

class Profile
{
public:
    explicit Profile(const std::filesystem::path &path);
    void save();

    void setBrightness(ushort value);
    void setButtonImage(ushort button, const std::string &cached_image_path);
    void setButtonLabel(ushort button, const std::string &label);
    void setButtonComponent(ushort button, const std::string &module_name, const std::string &component_name);

    std::optional<KeyProfile> getCurrentKeyProfile(ushort key);

    [[nodiscard]] std::vector<std::string> getPages() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::string getCurrentPageName() const;

private:
    std::filesystem::path m_path;

    std::string m_profile_name;
    ushort m_brightness{};

    struct Page
    {
        std::string m_name;
        std::map<ushort, KeyProfile> m_keys;
    };
    std::map<std::string, std::shared_ptr<Page>> m_pages; // <page name, page buttons>
    std::shared_ptr<Page> m_current_page;
};
