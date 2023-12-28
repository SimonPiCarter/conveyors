
# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

# Add a dynamic library called engine - this builds engine.dll
add_library(engine SHARED ${SOURCES})
target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(engine PUBLIC godot::cpp)
