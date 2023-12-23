cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/source/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/source/*.h)

add_library(streamdeck.lib SHARED ${HEADERS_FILES} ${SOURCE_FILES})
target_include_directories(streamdeck.lib PUBLIC include)
target_link_libraries(streamdeck.lib 
    PUBLIC
        Threads::Threads
        hidapi-libusb
        ${OpenCV_LIBS}
        jpeg
        png
        cppzmq)