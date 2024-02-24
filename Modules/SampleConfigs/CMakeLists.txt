cmake_minimum_required(VERSION 3.6)

add_library(streamdeck.modules.sample_config SHARED main.cpp)
target_link_libraries(streamdeck.modules.sample_config
    PUBLIC
        streamdeck.module_api
        streamdeck.lib
        asound)

install(TARGETS streamdeck.modules.sample_config DESTINATION $ENV{HOME}/.streamdeck/modules)
