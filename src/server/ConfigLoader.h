#pragma once 

#include <3rdparty/nlohmann/json.hpp>

#include "Profile.h"
#include "util.h"

static constexpr const char*const FOLDER_STREAMDECK = ".streamdeck";
static constexpr const char*const FOLDER_MODULES = "modules";
static constexpr const char*const FOLDER_IMAGES = "images";
static constexpr const char*const FOLDER_PROFILES = "profiles";
static constexpr const char*const FILENAME_CONFIG = ".config";
static constexpr const char*const FILENAME_DEFAULT_PROFILE = "default.profile";
static constexpr const char*const EXTENSION_PROFILE = ".profile";

void createDefaultConfigs(const std::filesystem::path &path);

Profile loadDeckProfile(const std::string &deck_serial);

std::vector<std::string> getDeckProfiles(const std::string &deck_serial);

std::filesystem::path saveButtonImageForDeck(const std::string &deck_serial, const std::vector<uint8_t> &image);