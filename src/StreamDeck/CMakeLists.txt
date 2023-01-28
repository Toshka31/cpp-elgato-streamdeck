cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_library(streamdeck.lib SHARED ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(streamdeck.lib 
    PUBLIC 
        streamdeck.iface
        hidapi-libusb 
        pthread
        ${OpenCV_LIBS}
        jpeg
        png )