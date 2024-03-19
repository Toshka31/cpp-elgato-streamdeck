cmake_minimum_required(VERSION 3.8)

project(streamDeck.api C CXX)

# Proto file
get_filename_component(api_proto "api.proto" ABSOLUTE)
get_filename_component(api_proto_path "${api_proto}" PATH)

# Generated sources
set(api_proto_dest_srcs "${CMAKE_CURRENT_LIST_DIR}/source/api.pb.cc")
set(api_proto_dest_hdrs "${CMAKE_CURRENT_LIST_DIR}/include/StreamDeck/API/api.pb.h")
set(api_grpc_dest_srcs "${CMAKE_CURRENT_LIST_DIR}/source/api.grpc.pb.cc")
set(api_grpc_dest_hdrs "${CMAKE_CURRENT_LIST_DIR}/include/StreamDeck/API/api.grpc.pb.h")

set(api_proto_out_srcs "${CMAKE_CURRENT_BINARY_DIR}/api.pb.cc")
set(api_proto_out_hdrs "${CMAKE_CURRENT_BINARY_DIR}/api.pb.h")
set(api_grpc_out_srcs "${CMAKE_CURRENT_BINARY_DIR}/api.grpc.pb.cc")
set(api_grpc_out_hdrs "${CMAKE_CURRENT_BINARY_DIR}/api.grpc.pb.h")

add_custom_target(generate_api
        COMMAND
            ${_PROTOBUF_PROTOC}
            --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${CMAKE_CURRENT_LIST_DIR}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${api_proto}"
        DEPENDS "${api_proto}"
)

configure_file(${api_proto_out_srcs} ${api_proto_dest_srcs} COPYONLY)
configure_file(${api_proto_out_hdrs} ${api_proto_dest_hdrs} COPYONLY)
configure_file(${api_grpc_out_srcs} ${api_grpc_dest_srcs} COPYONLY)
configure_file(${api_grpc_out_hdrs} ${api_grpc_dest_hdrs} COPYONLY)

add_library(${PROJECT_NAME}
        ${api_proto_dest_srcs}
        ${api_proto_dest_hdrs}
        ${api_grpc_dest_srcs}
        ${api_grpc_dest_hdrs})
add_dependencies(${PROJECT_NAME} generate_api)
target_link_libraries(${PROJECT_NAME}
        PUBLIC ${_REFLECTION}
        PUBLIC ${_GRPC_GRPCPP}
        PUBLIC ${_PROTOBUF_LIBPROTOBUF})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/StreamDeck/API)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)