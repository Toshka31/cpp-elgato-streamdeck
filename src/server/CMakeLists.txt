cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_executable(streamdeck.server ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(streamdeck.server 
    PUBLIC
        boost_system
        boost_filesystem
        streamdeck.iface
        streamdeck.lib
        rpc)

add_dependencies(streamdeck.server streamdeck.modules.alsa)