cmake_minimum_required(VERSION 3.0)
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

install(TARGETS emscripten
        ARCHIVE DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
        LIBRARY DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
        PUBLIC_HEADER DESTINATION ${CMAKE_SYSROOT}/include/faasm
        )

install(
        FILES emscripten.imports
        DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
        RENAME libemscripten.imports
)

target_include_directories(emscripten PUBLIC ${CMAKE_CURRENT_LIST_DIR})
