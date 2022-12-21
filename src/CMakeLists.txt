cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(SAMPLES_FOLDER="${SAMPLES_FOLDER}")

find_package(Boost) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
endif()

add_executable(test ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(test PUBLIC hidapi-libusb pthread jpeg)
