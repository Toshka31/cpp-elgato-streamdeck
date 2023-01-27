#pragma once

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <filesystem>

class Profile
{
public:
    explicit Profile(const std::filesystem::path &path);
    void save();

    void setBrightness(ushort value);

    void setButtonImage(ushort button, const std::string &cached_image_path);

    void setButtonComponent(ushort button, const std::string &module_name, const std::string &component_name);

    [[nodiscard]] std::vector<std::string> getPages() const;

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] std::string getCurrentPageName() const;

private:
    std::string m_profile_name;
    ushort m_brightness;

    struct Key {
        Key() = default;

        Key(std::string custom_image)
            : m_custom_image(std::move(custom_image))
        {}

        Key(std::string module_name,
            std::string component_name)
            : m_module_name(std::move(module_name)),
              m_component_name(std::move(component_name))
        {}

        std::string m_custom_image;
        std::string m_module_name;
        std::string m_component_name;
    };
    struct Page {
        std::string m_name;
        std::map<ushort, Key> m_keys;
    };
    std::map<std::string, std::shared_ptr<Page>> m_pages; // <page name, page buttons>
    std::shared_ptr<Page> m_current_page;
};
