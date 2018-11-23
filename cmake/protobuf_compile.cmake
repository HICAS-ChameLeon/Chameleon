# In this case, this function would:
#
#   (1)  given the drectory path where the proto file is, the name of proto file, and the output directory path of the relevant header and cpp, we will use protoc to
#         to compile it and generate the header and cpp in the directory path.
#         Finally, we use the generated header files and cpps to form a static library with ${OUT_PUT_LIBRARY} as the name.
#         For example,
#         PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/protobuf_begin/proto SearchRequest ${CMAKE_BINARY_DIR}/src)
#         PROTO_PATH: ${CMAKE_SOURCE_DIR}/src/protobuf_begin/proto
#         PROTO_NAME: SearchRequest
#         OUT_PUT_PATH: ${CMAKE_BINARY_DIR}/src
#         OUT_PUT_LIBRARY: foo
#         means that we will compile the ${CMAKE_SOURCE_DIR}/src/protobuf_begin/proto/SearchRequest.proto and generates
#         SearchRequest.pb.h and SearchRequest.pb.cc under the directory ${CMAKE_BINARY_DIR}/src.
#         Finally, we could generate a static library libfoo.a including the SearchRequest.pb.h and SearchRequest.pb.cc.

#   (2) export the following variables, based on the `PROTO_NAME` parameter
#       (a) SearchRequest_PROTO:    ${CMAKE_SOURCE_DIR}/src/protobuf_begin/proto/SearchRequest
#       (b) SearchRequest_PROTO_CC:       ${CMAKE_BINARY_DIR}/src/SearchRequest.pb.cc
#       (a) SearchRequest_PROTO_H:        ${CMAKE_BINARY_DIR}/src/SearchRequest.pb.h


#message(18 ${CMAKE_SOURCE_DIR})
function(PROTOC_COMPILE PROTO_PATH PROTO_NAME OUT_PUT_PATH OUT_PUT_LIBRARY)

    set(TO_INCLUDE_DIR
            --cpp_out=${OUT_PUT_PATH}
            --proto_path=${PROTO_PATH})

    # Names of variables we will be publicly exporting.
    set(PROTO_VAR ${PROTO_NAME}_PROTO)    # e.g., SearchRequest_PROTO
    set(CC_VAR    ${PROTO_NAME}_PROTO_CC) # e.g., SearchRequest_CC
    set(H_VAR     ${PROTO_NAME}_PROTO_H)  # e.g., SearchRequest_H
#        message(29${PROTO_VAR})

    # Fully qualified paths for the input .proto files and the output C files.
    set(PROTO ${PROTO_PATH}/${PROTO_NAME}.proto)
    set(CC    ${OUT_PUT_PATH}/${PROTO_NAME}.pb.cc)
    set(H     ${OUT_PUT_PATH}/${PROTO_NAME}.pb.h)

    # Export variables holding the target filenames.
    set(${PROTO_VAR} ${PROTO} PARENT_SCOPE) # e.g., SearchRequest.proto
    set(${CC_VAR}    ${CC}    PARENT_SCOPE) # e.g., SearchRequest.pb.cc
    set(${H_VAR}     ${H}     PARENT_SCOPE) # e.g., SearchRequest.pb.h

#    message("compile the .proto file")
    # Compile the .proto file.
    ADD_CUSTOM_COMMAND(
            OUTPUT ${CC} ${H}
            # ${protobuf_2_6_compiler} 定义在　protobuf_download.cmake 中
            COMMAND ${protobuf_2_6_compiler} ${TO_INCLUDE_DIR} ${PROTO}
            )
#    message("ends compile the .proto file ")

    # expose the library with ${OUT_PUT_LIBRARY} as the name formed from the generated headers and cpps
    add_library(${OUT_PUT_LIBRARY} ${CC} ${H})

endfunction()

