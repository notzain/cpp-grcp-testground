add_library(core)

target_include_directories(core
    PUBLIC 
        .
)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS *.h *.cpp)

target_sources(core
    PRIVATE
        ${src_files}
)

target_link_libraries(core
    PUBLIC 
        proto
        CONAN_PKG::boost
        CONAN_PKG::spdlog
        CONAN_PKG::nlohmann_json
        CONAN_PKG::pcapplusplus
        CONAN_PKG::expected-lite
)

target_precompile_headers(core PRIVATE pch.h)
