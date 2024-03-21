cmake_minimum_required(VERSION 3.6)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

add_library(streamdeck.modules.media SHARED main.cpp GDbusMediaWrapper.cpp)
target_link_libraries(streamdeck.modules.media
    PUBLIC
        streamdeck.module_api
        streamdeck.lib
        PkgConfig::deps
)

install(TARGETS streamdeck.modules.media DESTINATION $ENV{HOME}/.streamdeck/modules)