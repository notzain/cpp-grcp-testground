add_executable(app)

target_include_directories(app
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/generated
)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS *.h *.cpp)

target_sources(app
    PRIVATE 
        ${src_files}
)

target_link_libraries(app
    PRIVATE 
        core
        CONAN_PKG::docopt.cpp
)

if(NOT MSVC)
    add_custom_command(TARGET app 
        POST_BUILD
        COMMAND sudo -kS setcap cap_net_raw+ep ${CMAKE_BINARY_DIR}/bin/app
        COMMENT "Setting netcap capabilities on app"
    )
endif()
