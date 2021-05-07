cmake_minimum_required(VERSION 3.12)

project(
  Example_gRPC
  VERSION 0.1.0
  DESCRIPTION
    "Example project showing how to build with gRPC from Conan with CMake"
  HOMEPAGE_URL ""
  LANGUAGES CXX
)

set(WARNINGS_AS_ERRORS FALSE)

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/Cache.cmake)
include(cmake/Doxygen.cmake)
enable_doxygen()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CONAN_SYSTEM_INCLUDES ON)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan"
  )
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
  )
endif()
include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_add_remote(
  NAME
  bincrafters
  URL
  https://api.bintray.com/conan/bincrafters/public-conan
)

conan_cmake_run(
  CONANFILE
  conanfile.txt
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD
  missing
)

# Copy relevant configuration files to the build directory.
configure_file(.editorconfig .editorconfig COPYONLY)
configure_file(.clang-format .clang-format COPYONLY)

add_subdirectory(protos)
add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(tests)
