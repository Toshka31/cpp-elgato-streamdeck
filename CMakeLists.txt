cmake_minimum_required(VERSION 3.6)

project(elgato-streamdeck)

set(CMAKE_CXX_STANDARD 20)

option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)


find_package(Boost)
find_package(OpenCV REQUIRED)
find_package(rpclib REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(cppzmq)

add_library(3rdparty.nlohmann.iface INTERFACE)
target_include_directories(3rdparty.nlohmann.iface INTERFACE 3rdparty/include)

add_subdirectory(API)
add_subdirectory(StreamdeckLib)
add_subdirectory(ModuleSDK)
add_subdirectory(Modules)
add_subdirectory(Server)
add_subdirectory(CLI)
add_subdirectory(UI)
add_subdirectory(DeviceEmulator)

