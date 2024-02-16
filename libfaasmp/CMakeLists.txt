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

    set(LLVM_OMP_SRC_DIR "/opt/llvm-project/openmp/runtime/src")
    set(LLVM_OMP_BUILD_SRC_DIR "/opt/llvm-project/build/llvm/projects/openmp/runtime/src")
    # set(LIBOMP_ASMFILES "${LLVM_OMP_SRC_DIR}/z_Linux_asm.S")
    set(LIBOMP_ASMFILES "z_Linux_asm.S")
    set(LIBOMP_HEADERS "${LLVM_OMP_BUILD_SRC_DIR}/omp.h")
    set_source_files_properties(${LIBOMP_ASMFILES} PROPERTIES LANGUAGE C)
    set(LIB_FILES ${LIBOMP_ASMFILES} ${LIB_FILES})

    add_library(faasmp STATIC ${LIB_FILES})
    set_target_properties(faasmp PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
    # target_include_directories(faasmp PUBLIC ${LLVM_OMP_BUILD_SRC_DIR} ${LLVM_OMP_SRC_DIR})

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

    # Add omp.h header copied from clang source
    # TODO: can we copy it directly from LLVM source, for each LLVM version?
    install(
        FILES ${LIBOMP_HEADERS}
        DESTINATION $ENV{FAASM_WASM_HEADER_INSTALL_DIR}
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
