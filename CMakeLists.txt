cmake_minimum_required(VERSION 3.16)
project(OpenGLProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

add_subdirectory(Dependencies/glfw EXCLUDE_FROM_ALL)
add_subdirectory(Dependencies/glew/build/cmake EXCLUDE_FROM_ALL)

file(GLOB OPENGLPROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)

find_package(OpenGL REQUIRED)

add_executable(OpenGLProject ${OPENGLPROJECT_SOURCES})

#target_compile_definitions(OpenGLProject PRIVATE GLEW_STATIC)

target_include_directories(OpenGLProject PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include
)

target_link_libraries(OpenGLProject PRIVATE
    glfw
    OpenGL::GL
    glew
)