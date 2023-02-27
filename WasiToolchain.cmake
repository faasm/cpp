cmake_minimum_required(VERSION 3.4.0)

# -----------------------------------------
# A useful reference for this file is the wasi-sdk equivalent:
# https://github.com/WebAssembly/wasi-sdk/blob/main/wasi-sdk.cmake
#
# This toolchain file relies on the variable definitions in faasmtools/build.py
# which we pass as environment variables to CMake, Make, or configure
# -----------------------------------------

set(INSTALL_DIR $ENV{FAASM_WASM_INSTALL_DIR})
set(FAASM_SYSROOT $ENV{FAASM_WASM_SYSROOT})

set(EXE_SUFFIX "")

set(UNIX 1)

# Note that system name and processor here are crucial
# Setting system name automatically switches on cross-compiling
set(CMAKE_SYSTEM_NAME Wasm)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR wasm32)

if(FAASM_BUILD_SHARED)
    message(STATUS "Faasm building SHARED libraries")

    # See note in README about shared libraries targeting Emscripten
    set(WASM_TRIPLE $ENV{FAASM_WASM_HOST_SHARED})
else()
    message(STATUS "Faasm building STATIC libraries")

    set(WASM_TRIPLE $ENV{FAASM_WASM_HOST_STATIC})
endif()

set(CMAKE_C_COMPILER_TARGET ${WASM_TRIPLE} CACHE STRING "faasm build")
set(CMAKE_CXX_COMPILER_TARGET ${WASM_TRIPLE} CACHE STRING "faasm build")
message(STATUS "Faasm building target ${CMAKE_CXX_COMPILER_TARGET}")

# Specify LLVM toolchain
set(CMAKE_C_COMPILER $ENV{FAASM_WASM_CC})
set(CMAKE_CXX_COMPILER $ENV{FAASM_WASM_CXX})
set(CMAKE_AR $ENV{FAASM_WASM_AR} CACHE STRING "faasm build")
set(CMAKE_NM $ENV{FAASM_WASM_NM} CACHE STRING "faasm build")
set(CMAKE_RANLIB $ENV{FAASM_WASM_RANLIB} CACHE STRING "faasm build")

# We define dynamic linking functions in Faasm
unset(CMAKE_DL_LIBS CACHE)

# Add definition for flagging Faasm
add_definitions(-D__faasm)

set(FAASM_COMPILER_FLAGS $ENV{FAASM_WASM_CFLAGS})

if(FAASM_BUILD_SHARED)
    set(FAASM_COMPILER_FLAGS "${FAASM_COMPILER_FLAGS} $ENV{FAASM_WASM_CFLAGS_SHARED}")
endif()

set(CMAKE_SYSROOT ${FAASM_SYSROOT} CACHE STRING "faasm build")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FAASM_COMPILER_FLAGS}" CACHE STRING "faasm build")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FAASM_COMPILER_FLAGS}" CACHE STRING "faasm build")
set(CMAKE_LINKER_FLAGS $ENV{FAASM_WASM_LINKER_FLAGS} CACHE STRING "faasm build")
set(CMAKE_SHARED_LINKER_FLAGS $ENV{FAASM_WASM_SHARED_LINKER_FLAGS} CACHE STRING "faasm build")
set(CMAKE_EXE_LINKER_FLAGS $ENV{FAASM_WASM_EXE_LINKER_FLAGS} CACHE STRING "faasm build")

# This is important to ensure the right search path
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)

set(FAASM_BLAS_LIBS $ENV{FAASM_WASM_BLAS_LIBS})

