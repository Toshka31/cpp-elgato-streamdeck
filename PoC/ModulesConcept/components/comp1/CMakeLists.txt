cmake_minimum_required(VERSION 3.2)

add_library(comp1 SHARED main.cpp)
target_link_libraries(comp1 SDK.iface)