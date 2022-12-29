
add_library(SDK.iface INTERFACE)
target_include_directories(SDK.iface INTERFACE ./)

add_library(SDK STATIC module.cpp)
target_include_directories(SDK PUBLIC ./)
target_compile_options(SDK PRIVATE -fPIC)