cmake_minimum_required(VERSION 3.6)

project(elgato-streamdeck)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost)
find_package(OpenCV REQUIRED)
find_package(rpclib REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(cppzmq)

add_library(3rdparty.nlohmann.iface INTERFACE)
target_include_directories(3rdparty.nlohmann.iface INTERFACE 3rdparty/include)

add_subdirectory(StreamdeckLib)
add_subdirectory(ModuleSDK)
add_subdirectory(Modules)
add_subdirectory(Server)
add_subdirectory(CLI)
add_subdirectory(UI)
add_subdirectory(DeviceEmulator)

