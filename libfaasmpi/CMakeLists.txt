cmake_minimum_required(VERSION 3.0)
project(faasmpi)

# ------------------------------------------
# This builds the Faabric MPI library to wasm and
# adds the relevant .imports file
# ------------------------------------------

set(CMAKE_CXX_STANDARD 17)

set(PUBLIC_HEADERS
    faasmpi/faasmpi.h
)

set(LIB_FILES
    faasmpi.cpp
)

# Only do the install if we're doing a WASM build
# WARNING - this overwrites the existing mpi.h on the system if not used in a
# wasm build
if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(STATUS "Libfaasmpi WebAssembly build")

    add_library(faasmpi STATIC ${LIB_FILES})
    set_target_properties(faasmpi PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    # Add Faabric dependency
    set(FAABRIC_SOURCE_DIR ${CMAKE_BINARY_DIR}/../../third-party/faabric/src/mpi)
    set(FAABRIC_INCLUDE_DIR ${CMAKE_BINARY_DIR}/../../third-party/faabric/include)
    set(FAABRIC_BINARY_DIR ${FAABRIC_SOURCE_DIR}/build)
    add_subdirectory(${FAABRIC_SOURCE_DIR} ${FAABRIC_BINARY_DIR})
    add_dependencies(faasmpi faabricmpi)
    install(FILES ${FAABRIC_BINARY_DIR}/libfaabricmpi.a
            DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
    )

    # Install in sysroot
    install(TARGETS faasmpi
            ARCHIVE DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            LIBRARY DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            PUBLIC_HEADER DESTINATION ${CMAKE_SYSROOT}/include/faasmpi
    )

    # Add imports
    install(FILES faasmpi.imports
            DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            RENAME libfaasmpi.imports
    )

    # Symlink header
    install(CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        ${FAABRIC_INCLUDE_DIR}/faabric/mpi/mpi.h \
        ${CMAKE_SYSROOT}/include/mpi.h   \
        )"
    )

    # Symlink library
    install(CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libfaabricmpi.a \
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libmpi.a   \
        )"
    )
endif()
