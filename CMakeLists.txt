# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.8)

# Set this project's name.
project("tls_client_server")

# Set the C++ standard to use to build this target.
set(CMAKE_CXX_STANDARD 20) 

#generate debug symbols
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

