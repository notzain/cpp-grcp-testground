add_executable(app-cross)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS *.h *.cpp)

target_sources(app-cross
    PRIVATE 
        ${src_files}
)