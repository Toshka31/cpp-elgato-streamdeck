cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_library(streamdeck.server.private.iface INTERFACE)
target_include_directories(streamdeck.server.private.iface INTERFACE include)

add_executable(streamdeck.server ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(streamdeck.server 
    PUBLIC
        boost_system
        boost_filesystem
        streamdeck.lib
        streamdeck.module_api
        streamdeck.server.private.iface
        OpenSSL::Crypto
        rpc
        3rdparty.nlohmann.iface)

if (USE_EMULATOR)
    target_compile_definitions(streamdeck.server PUBLIC EMULATOR)
endif()