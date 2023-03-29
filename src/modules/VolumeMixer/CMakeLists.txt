cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.hpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)

include_directories (${GTK3_INCLUDE_DIRS})
link_directories (${GTK3_LIBRARY_DIRS})

add_library(streamdeck.modules.mixer SHARED ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(streamdeck.modules.mixer
        pulse
        ${GTK3_LIBRARIES}
        streamdeck.iface
        streamdeck.lib)

install(TARGETS streamdeck.modules.mixer DESTINATION $ENV{HOME}/.streamdeck/modules)