#pragma once

#include <filesystem>

#include <pwd.h>
#include <unistd.h>

namespace util {

    std::filesystem::path getHomeDirectory()
    {
        struct passwd *pw = getpwuid(getuid());
        return std::filesystem::path(pw->pw_dir);
    }

}
