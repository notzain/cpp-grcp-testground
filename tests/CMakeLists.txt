add_executable(tests)

set(src_files main.cpp core/net/IPv4AddressTest.cpp
              core/util/type/WrapAround.cpp
)

target_sources(tests PRIVATE ${src_files})

target_link_libraries(tests PRIVATE core CONAN_PKG::doctest)

# set_target_properties(tests PROPERTIES UNITY_BUILD ON)
