cmake_minimum_required(VERSION 3.12)

project(Example_gRPC
        VERSION 0.1.0
        DESCRIPTION "Example project showing how to build with gRPC from Conan with CMake"
        HOMEPAGE_URL ""
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options("/std:c++latest")
else()
    set(CMAKE_CXX_STANDARD 17)
endif()

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_add_remote(NAME bincrafters
                URL https://api.bintray.com/conan/bincrafters/public-conan)
conan_add_remote(NAME inexorgame
                URL https://api.bintray.com/conan/inexorgame/inexor-conan)

conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

# Let Conan do it's magic.
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup(TARGETS)

# Copy relevant configuration files to the build directory.
configure_file(.editorconfig .editorconfig COPYONLY)
configure_file(.clang-format .clang-format COPYONLY)

add_subdirectory(protos)
add_subdirectory(core)
add_subdirectory(app)
