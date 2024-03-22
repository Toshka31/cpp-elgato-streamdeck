#include "util/SystemUtil.h"

#include <pwd.h>
#include <unistd.h>

namespace system_util {

std::filesystem::path getHomeDirectory() {
    struct passwd *pw = getpwuid(getuid());
    return {pw->pw_dir};
}

}
