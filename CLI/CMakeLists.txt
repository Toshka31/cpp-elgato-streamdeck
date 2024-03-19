cmake_minimum_required(VERSION 3.6)

file(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_executable(streamdeck.cli ${HEADERS_FILES} ${SOURCE_FILES})
target_link_libraries(streamdeck.cli PUBLIC
        streamdeck.lib
        streamDeck.api
        rpc
        ${OpenCV_LIBS})