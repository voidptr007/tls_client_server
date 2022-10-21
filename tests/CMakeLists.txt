# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.8)

# Set the C++ standard to use to build this target.
set(CMAKE_CXX_STANDARD 20) 

#generate debug symbols
set(CMAKE_BUILD_TYPE Debug)

#include headers from include folder
include_directories(${PROJECT_SOURCE_DIR}/include)

#define global variable for all cpp files
file(GLOB TLSSERVER_SOURCES
            "${PROJECT_SOURCE_DIR}/src/*.cpp"
            "${PROJECT_SOURCE_DIR}/tests/test_tlsserver.cpp")

file(GLOB TLSCLIENT_SOURCES
            "${PROJECT_SOURCE_DIR}/src/*.cpp"
            "${PROJECT_SOURCE_DIR}/tests/test_tlsclient.cpp")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

#compile and build exe for source files
add_executable(tlsserver ${TLSSERVER_SOURCES})
add_executable(tlsclient ${TLSCLIENT_SOURCES})

#link libraries
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
target_link_libraries(tlsserver OpenSSL::Crypto)
target_link_libraries(tlsclient OpenSSL::Crypto)
target_link_libraries(tlsserver OpenSSL::SSL)
target_link_libraries(tlsclient OpenSSL::SSL)
target_link_libraries(tlsserver pthread)
target_link_libraries(tlsclient pthread)
