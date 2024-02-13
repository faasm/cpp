cmake_minimum_required(VERSION 3.8)
project(libemscripten)

# ------ Support for certain emscripten functions ------ #

set(CMAKE_CXX_STANDARD 17)

set(PUBLIC_HEADERS
    faasm/emscripten.h
)

set(LIB_FILES
    emscripten.c
)

add_library(emscripten STATIC ${LIB_FILES})
set_target_properties(emscripten PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(
    TARGETS emscripten
    ARCHIVE DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
    LIBRARY DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
    PUBLIC_HEADER DESTINATION $ENV{FAASM_WASM_HEADER_INSTALL_DIR}/faasm
)

install(
    FILES libemscripten.imports
    DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
    RENAME libemscripten.imports
)

target_include_directories(emscripten PUBLIC ${CMAKE_CURRENT_LIST_DIR})
