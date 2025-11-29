cmake_minimum_required(VERSION 3.10)
project(boom)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLFW
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Add executable
add_executable(boom
        main.cc
        Canvas.cpp
        main.cc
)

# Link libraries
target_link_libraries(boom
        glfw
        OpenGL::GL
)

# Include directories
target_include_directories(boom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
