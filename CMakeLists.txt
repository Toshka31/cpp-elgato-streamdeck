cmake_minimum_required(VERSION 3.2)

project(elgato-streamdeck)

set(SAMPLES_FOLDER ${CMAKE_CURRENT_LIST_DIR}/sample_images)

add_library(streamdeck.iface INTERFACE)
target_include_directories(streamdeck.iface INTERFACE include)

add_subdirectory(src)

add_subdirectory(PoC/ModulesConcept)