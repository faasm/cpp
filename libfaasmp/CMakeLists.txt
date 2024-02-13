cmake_minimum_required(VERSION 3.8)
project(libfaasmp)

# ------ Faasm OpenMP library ------ #

set(CMAKE_CXX_STANDARD 17)

set(PUBLIC_HEADERS
    faasmp/faasmp.h
)

set(LIB_FILES
    faasmp.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "WASI")
    message(STATUS "Libfaasmp WebAssembly build")

    set(LIBOMP_ASMFILES "z_Linux_asm.S")
    set_source_files_properties(${LIBOMP_ASMFILES} PROPERTIES LANGUAGE C)
    set(LIB_FILES ${LIBOMP_ASMFILES} ${LIB_FILES})

    add_library(faasmp STATIC ${LIB_FILES})
    set_target_properties(faasmp PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    # Install in WASM sysroot
    install(
        TARGETS faasmp
        ARCHIVE DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        LIBRARY DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION $ENV{FAASM_WASM_HEADER_INSTALL_DIR}/faasmmp
    )

    # Add imports
    install(
        FILES libfaasmp.imports
        DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        RENAME libfaasmp.imports
    )
else ()
    message(STATUS "Libfaasmp native build")

    if(BUILD_SHARED_LIBS)
        add_library(faasmp SHARED ${LIB_FILES})
    else()
        add_library(faasmp STATIC ${LIB_FILES})
    endif()

    set_target_properties(faasmp PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    install(
        TARGETS faasmp
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/faasmp
    )
endif ()
