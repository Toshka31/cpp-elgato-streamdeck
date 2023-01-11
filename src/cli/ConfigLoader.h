#pragma once 

#include <3rdparty/nlohmann/json.hpp>

static constexpr const char*const FOLDER_STREAMDECK = ".streamdeck";
static constexpr const char*const FOLDER_MODULES = "modules";
static constexpr const char*const FOLDER_PROFILES = "profiles";
static constexpr const char*const FILENAME_CONFIG = ".config";
static constexpr const char*const FILENAME_DEFAULT_PROFILE = "default.profile";
static const nlohmann::json DEFAULT_PROFILE = R"(
{
	"brightness": 25,
	"page": "Page 1",
	"pages": [{
		"name" : "Page 1",
		"keys" : []
	}]
}
)"_json;

using namespace std::chrono_literals;

void createDefaultConfigs(const std::filesystem::path &path)
{
    std::filesystem::path path_default_profile = path / FILENAME_DEFAULT_PROFILE;
    std::ofstream file_profile(path_default_profile);
    file_profile << DEFAULT_PROFILE;
    file_profile.close();

    std::filesystem::path path_default_config = path / FILENAME_CONFIG;
    std::ofstream file_config(path_default_config);
    file_config << FILENAME_DEFAULT_PROFILE;
    file_config.close();
}

Profile loadDeckConfiguration(const std::string &deck_serial, const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
        std::filesystem::create_directory(path);

    std::filesystem::path deck_folder_path = path / deck_serial;

    if (!std::filesystem::exists(deck_folder_path))
    {
        std::filesystem::create_directory(deck_folder_path);
        createDefaultConfigs(deck_folder_path);
    }

    std::filesystem::path path_default_config = deck_folder_path / FILENAME_CONFIG;
    std::ifstream file_config(path_default_config);
    std::string config_name;
    file_config >> config_name;

    Profile profile(deck_folder_path / config_name);
    return profile;
}