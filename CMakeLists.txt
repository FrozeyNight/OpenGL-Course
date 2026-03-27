cmake_minimum_required(VERSION 3.16)
project(OpenGLProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

add_subdirectory(Dependencies/glfw EXCLUDE_FROM_ALL)

file(GLOB OPENGLPROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

find_package(OpenGL REQUIRED)

add_executable(OpenGLProject ${OPENGLPROJECT_SOURCES})

target_include_directories(OpenGLProject PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(OpenGLProject PRIVATE
    glfw
    OpenGL::GL
)