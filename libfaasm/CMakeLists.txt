cmake_minimum_required(VERSION 3.0)
project(libfaasm)

# ------ Faasm C/C++ library ------ #

set(CMAKE_CXX_STANDARD 17)

set(PUBLIC_HEADERS
        faasm/core.h
        faasm/host_interface.h
        faasm/array.h
        faasm/compare.h
        faasm/counter.h
        faasm/faasm.h
        faasm/files.h
        faasm/input.h
        faasm/matrix.h
        faasm/print.h
        faasm/random.h
        faasm/sgd.h
        faasm/state.h
        faasm/time.h
        )

set(LIB_FILES
        compare.cpp
        core.cpp
        counter.cpp
        files.cpp
        input.cpp
        matrix.cpp
        print.cpp
        random.cpp
        sgd.cpp
        state.cpp
        time.cpp
        zygote.cpp
        )


if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(STATUS "Libfaasm WebAssembly build")

    add_library(faasm STATIC ${LIB_FILES})
    set_target_properties(faasm PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    # Install in WASM sysroot
    install(TARGETS faasm
            ARCHIVE DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            LIBRARY DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            PUBLIC_HEADER DESTINATION ${CMAKE_SYSROOT}/include/faasm
            )

    # Add allowed import files
    install(
            FILES faasm.imports
            DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            RENAME libfaasm.imports
    )
else ()
    message(STATUS "Libfaasm native build")

    if(BUILD_SHARED_LIBS)
        add_library(faasm SHARED ${LIB_FILES})
    else()
        add_library(faasm STATIC ${LIB_FILES})
    endif()

    set_target_properties(faasm PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    install(TARGETS faasm
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include/faasm
            )
endif ()

target_include_directories(faasm PUBLIC ${CMAKE_CURRENT_LIST_DIR})
