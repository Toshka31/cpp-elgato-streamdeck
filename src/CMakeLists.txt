cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(SAMPLES_FOLDER="${SAMPLES_FOLDER}")

find_package(Boost)
find_package(OpenCV REQUIRED)
find_package(rpclib REQUIRED)
find_package(OpenSSL REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

add_subdirectory(StreamDeck)
add_subdirectory(modules)
add_subdirectory(server)
add_subdirectory(cli)
add_subdirectory(UI)

