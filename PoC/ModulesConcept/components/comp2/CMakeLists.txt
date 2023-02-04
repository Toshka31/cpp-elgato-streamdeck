cmake_minimum_required(VERSION 3.2)

add_library(comp2 SHARED module.cpp main.cpp comp1.cpp)
target_link_libraries(comp2 SDK.iface)