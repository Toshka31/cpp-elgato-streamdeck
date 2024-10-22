#include "ConfigLoader.h"

#include <3rdparty/nlohmann/json.hpp>

#include "Profile.h"

#include <fstream>

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

void createEmptyProfile(const std::filesystem::path &path)
{
    std::string str_path = path.string();
    std::ofstream file_profile(str_path);
    file_profile << DEFAULT_PROFILE;
    file_profile.close();
}

void createDefaultConfigs(const std::filesystem::path &path)
{
    std::string filename(std::string() + FILENAME_DEFAULT_PROFILE_NAME + EXTENSION_PROFILE);
    createEmptyProfile(path / filename);

    std::filesystem::path path_default_config = path / FILENAME_CONFIG;
    std::ofstream file_config(path_default_config);
    file_config << FILENAME_DEFAULT_PROFILE_NAME;
    file_config.close();
}

std::filesystem::path getStreamDeckFolderPath(const std::string &deck_serial)
{
    std::filesystem::path configs_path = system_util::getHomeDirectory() / FOLDER_STREAMDECK;

    if (!std::filesystem::exists(configs_path))
        std::filesystem::create_directory(configs_path);

    std::filesystem::path deck_folder_path = configs_path / deck_serial;

    return deck_folder_path;
}

std::string getDefaultProfileName(const std::string &deck_serial)
{
    auto deck_folder_path = getStreamDeckFolderPath(deck_serial);

    std::filesystem::path path_default_config = deck_folder_path / FILENAME_CONFIG;
    std::ifstream file_config(path_default_config);
    if (file_config) {
        std::string config_name;
        file_config >> config_name;
        return config_name;
    }
    else
        return FILENAME_DEFAULT_PROFILE_NAME;
}

Profile loadDeckProfile(const std::string &deck_serial, const std::string &profile_name)
{
    auto deck_folder_path = getStreamDeckFolderPath(deck_serial);

    if (!std::filesystem::exists(deck_folder_path))
    {
        std::filesystem::create_directory(deck_folder_path);
        createDefaultConfigs(deck_folder_path);
    }

    Profile profile(deck_folder_path / (profile_name + EXTENSION_PROFILE));
    return profile;
}

Profile createNewProfile(const std::string &deck_serial, const std::string &profile_name)
{
    auto deck_folder_path = getStreamDeckFolderPath(deck_serial);

    // TODO check if exist and throw error
    createEmptyProfile(deck_folder_path / (profile_name + EXTENSION_PROFILE) );

    Profile profile(deck_folder_path / (profile_name + EXTENSION_PROFILE) );
    return profile;
}

std::vector<std::string> getDeckProfiles(const std::string &deck_serial)
{
    auto deck_folder_path = getStreamDeckFolderPath(deck_serial);

    if (!std::filesystem::exists(deck_folder_path))
    {
        std::filesystem::create_directory(deck_folder_path);
        createDefaultConfigs(deck_folder_path);
    }

    std::vector<std::string> result;
    for (auto const& dir_entry : std::filesystem::directory_iterator{deck_folder_path})
    {
        const auto& path = dir_entry.path();
        if (path.extension() == EXTENSION_PROFILE)
            result.push_back(path.stem());
    }
    return result;
}

#include <openssl/evp.h>

std::string calculateMD5(const std::vector<uint8_t> &data)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data.data(), data.size());
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::uppercase;
    for (int i = 0; i < md_len; ++i)
        ss << std::setw(2) << static_cast<int>(md_value[i]);
    return ss.str();
}

std::filesystem::path saveButtonImageForDeck(const std::string &deck_serial, const std::vector<uint8_t> &image)
{
    std::string name = calculateMD5(image);
    std::filesystem::path cache_images_path = system_util::getHomeDirectory() / FOLDER_STREAMDECK / FOLDER_IMAGES;
    if (!std::filesystem::exists(cache_images_path))
        std::filesystem::create_directories(cache_images_path);
    cache_images_path /= name;
    if (!std::filesystem::exists(cache_images_path)) {
        std::ofstream cache_file(cache_images_path, std::ios::binary | std::ios::out);
        cache_file.write(reinterpret_cast<char const *>(image.data()), image.size());
    }
    return cache_images_path;
}