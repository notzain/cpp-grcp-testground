add_executable(app main.cpp)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_link_libraries(
  app PRIVATE core CONAN_PKG::docopt.cpp CONAN_PKG::backward-cpp
)

if(NOT MSVC)
  add_custom_command(
    TARGET app
    POST_BUILD
    COMMAND sudo -kS setcap cap_net_raw+ep ${CMAKE_BINARY_DIR}/bin/app
    COMMENT "Setting netcap capabilities on app"
  )
endif()

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
  set_property(TARGET app PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
else()
  message(FATAL_ERROR "Could not find the program include-what-you-use")
endif()
