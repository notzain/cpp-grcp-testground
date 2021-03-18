add_executable(tests)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS *.cpp)

target_sources(tests
    PRIVATE
        ${src_files}
)

target_link_libraries(tests
    PUBLIC 
        core
        CONAN_PKG::doctest
)

# set_target_properties(tests PROPERTIES UNITY_BUILD ON)