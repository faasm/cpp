cmake_minimum_required(VERSION 3.13.0)
project(faasm-cpp-examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Here we create a dummy library to check linking with static/ shared objects.
# We must link directly to the installed libraries so that we replicate the
# behaviour of external projects.
if(BUILD_SHARED_LIBS)
    add_library(native_lib SHARED native_lib.cpp)
    target_link_libraries(native_lib 
        libfaasm.so 
        libfaabric.so 
        libemulator.so
        libprotobuf.so
        )
else()
    add_library(native_lib STATIC native_lib.cpp)
    target_link_libraries(native_lib 
        libfaasm.a 
        libfaabric.a 
        libemulator.a
        libprotobuf.a
        )
endif()

# Include the native install of the libraries
target_include_directories(native_lib PUBLIC /usr/local/faasm/native/include)
target_link_directories(native_lib PUBLIC /usr/local/faasm/native/lib)

add_executable(native_exe native_exe.cpp)
target_link_libraries(native_exe native_lib)
