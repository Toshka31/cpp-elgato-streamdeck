cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_executable(test ${SOURCE_FILES})
target_link_libraries(test PUBLIC hidapi-libusb pthread)