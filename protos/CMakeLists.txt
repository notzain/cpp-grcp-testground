# Command to generate C++ gRPC files.
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

if(MSVC)
  set(protoc ${CMAKE_BINARY_DIR}/build_tools/protoc.exe)
  set(grpc_plugin ${CMAKE_BINARY_DIR}/build_tools/grpc_cpp_plugin.exe)
else()
  set(protoc ${CMAKE_BINARY_DIR}/build_tools/protoc)
  set(grpc_plugin ${CMAKE_BINARY_DIR}/build_tools/grpc_cpp_plugin)
endif()

set(hw_protos byeworld.proto helloworld.proto)

function(
  generate_file
  list
  suffix
  output
)
  set(${output} "")
  foreach(item ${list})
    get_filename_component(filename ${item} NAME_WE)
    list(APPEND output ${CMAKE_BINARY_DIR}/generated/${filename}.${suffix})
  endforeach()
  set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
  set(${output} PARENT_SCOPE)
endfunction()

set(hw_proto_srcs "")
set(hw_proto_hdrs "")
set(hw_grpc_srcs "")
set(hw_grpc_hdrs "")

generate_file("${hw_protos}" "pb.cc" hw_proto_srcs)
generate_file("${hw_protos}" "pb.h" hw_proto_hdrs)
generate_file("${hw_protos}" "grpc.pb.cc" hw_grpc_srcs)
generate_file("${hw_protos}" "grpc.pb.h" hw_grpc_hdrs)

add_custom_command(
  OUTPUT ${hw_proto_srcs}
         ${hw_proto_hdrs}
         ${hw_grpc_srcs}
         ${hw_grpc_hdrs}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND ${protoc} ARGS --grpc_out "${CMAKE_BINARY_DIR}/generated" --cpp_out
          "${CMAKE_BINARY_DIR}/generated" -I "${PROJECT_SOURCE_DIR}/protos"
          --plugin=protoc-gen-grpc=${grpc_plugin} ${hw_protos}
  DEPENDS ${hw_protos}
  COMMENT "Invoking protoc to generate gRPC C++ files."
)

# Custom target for generating C++ gRPC files that other targets can depend on.
add_custom_target(
  generate_grpc_files
  DEPENDS "${hw_proto_srcs}"
          "${hw_proto_hdrs}"
          "${hw_grpc_srcs}"
          "${hw_grpc_hdrs}"
)

add_library(proto ${hw_proto_srcs} ${hw_grpc_srcs})

target_link_libraries(proto CONAN_PKG::grpc)

target_include_directories(proto PUBLIC ${CMAKE_BINARY_DIR}/generated)

# add_dependencies(proto generate_grpc_files)
