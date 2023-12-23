cmake_minimum_required(VERSION 3.6)

add_library(streamdeck.modules.alsavolume SHARED main.cpp)
target_link_libraries(streamdeck.modules.alsavolume
    PUBLIC
        streamdeck.iface
        streamdeck.lib
        asound)

install(TARGETS streamdeck.modules.alsavolume DESTINATION $ENV{HOME}/.streamdeck/modules)
