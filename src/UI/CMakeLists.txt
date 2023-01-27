cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(streamdeck.UI ${HEADERS_FILES} ${SOURCE_FILES})

target_link_libraries(streamdeck.UI
        streamdeck.iface
        rpc
        Qt6::Widgets)