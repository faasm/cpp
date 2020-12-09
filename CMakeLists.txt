cmake_minimum_required(VERSION 3.13.0)
project(faasm-cpp)

# Top-level CMake config
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (BUILD_SHARED_LIBS AND CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(FATAL_ERROR "Building Wasm shared libs not currently supported")
endif()

# Add Faabric dependency
add_subdirectory(third-party/faabric)

# Generated protobuf headers
include_directories(${CMAKE_CURRENT_BINARY_DIR}/third-party/faabric/src)

add_subdirectory(libfaasm)
add_subdirectory(emulator)
add_subdirectory(examples)
add_subdirectory(tests)
