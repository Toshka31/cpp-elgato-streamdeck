cmake_minimum_required(VERSION 3.2)

add_library(streamdeck.modules.alsa SHARED main.cpp)
target_link_libraries(streamdeck.modules.alsa 
    PUBLIC
        streamdeck.iface
        streamdeck.lib
        asound)