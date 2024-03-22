#ifndef PROVIDEDPROFILE_H
#define PROVIDEDPROFILE_H

#include <map>
#include <string>
#include <utility>

// Mapped key to component from this module
struct ProvidedProfile {
    ProvidedProfile() = default;

    explicit ProvidedProfile(std::map<unsigned short, std::string> keys)
        : key_mapping(std::move(keys)) {};

    std::map<unsigned short, std::string> key_mapping;
};

#endif //PROVIDEDPROFILE_H
