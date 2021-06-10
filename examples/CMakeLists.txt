cmake_minimum_required(VERSION 3.13.0)
project(faasm-cpp-examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(BUILD_SHARED_LIBS)
    add_library(native_lib SHARED native_lib.cpp)
else()
    add_library(native_lib STATIC native_lib.cpp)
endif()

target_link_libraries(native_lib 
    faasm 
    emulator
    faabric
    faabricmpi
    zmq
    protobuf
    pthread
    pistache
    hiredis
    grpc++
    grpc++_reflection
    boost_system
    boost_filesystem
)

# Include the native install of the libraries
target_include_directories(native_lib PUBLIC /usr/local/faasm/native/include)
target_link_directories(native_lib PUBLIC /usr/local/faasm/native/lib)

add_executable(native_exe native_exe.cpp)
target_link_libraries(native_exe native_lib)
