cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(SAMPLES_FOLDER="${SAMPLES_FOLDER}")

find_package(Boost) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
endif()

add_subdirectory(StreamDeck)
add_subdirectory(cli)
