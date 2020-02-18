cmake_minimum_required(VERSION 3.10)
project(sugi C)
# Setting output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Libraries
find_package(Lua 5.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
# Headers
include_directories(
    include 
    ${LUA_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    )
# Source files
file(GLOB SOURCES "src/*.c")
# Create exec
add_executable(sugi ${SOURCES})
# Link libraries
target_link_libraries(
    sugi
    ${LUA_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    )
target_compile_options(sugi PRIVATE -w)
