cmake_minimum_required(VERSION 3.8)
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

if(BUILD_SHARED_LIBS)
    add_library(faasmpi SHARED ${LIB_FILES})
else()
    add_library(faasmpi STATIC ${LIB_FILES})
endif()

set_target_properties(faasmpi PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

# Install Faabric MPI library
set(FAABRIC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../third-party/faabric/src/mpi)
set(FAABRIC_BINARY_DIR ${FAABRIC_SOURCE_DIR}/build)

add_subdirectory(${FAABRIC_SOURCE_DIR} ${FAABRIC_BINARY_DIR})
add_dependencies(faasmpi faabricmpi)

if (CMAKE_SYSTEM_NAME STREQUAL "WASI")
    message(STATUS "Libfaasmpi WebAssembly build")

    install(
        FILES ${FAABRIC_BINARY_DIR}/libfaabricmpi.a
        DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
    )

    install(
        TARGETS faasmpi
        ARCHIVE DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        LIBRARY DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        PUBLIC_HEADER DESTINATION $ENV{FAASM_WASM_HEADER_INSTALL_DIR}/faasmmpi
    )

    # Add imports file
    install(
        FILES libfaasmpi.imports
        DESTINATION $ENV{FAASM_WASM_LIB_INSTALL_DIR}
        RENAME libfaasmpi.imports
    )

    # Symlink library as libmpi
    install(
        CODE "execute_process( \
        COMMAND
            ${CMAKE_COMMAND} -E create_symlink \
            $ENV{FAASM_WASM_LIB_INSTALL_DIR}/libfaabricmpi.a \
            $ENV{FAASM_WASM_LIB_INSTALL_DIR}/libmpi.a \
        )"
    )

    # Symlink mpi.h
    install(
        CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
            $ENV{FAASM_WASM_LIB_INSTALL_DIR}/include/faabric/mpi/mpi.h \
            $ENV{FAASM_WASM_HEADER_INSTALL_DIR}/mpi.h   \
        )"
    )
else()
    message(STATUS "Libfaasmpi native build")

    install(
        TARGETS faasmpi
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/faasmpi
    )

    # Symlink mpi.h
    install(
        CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
            ${CMAKE_INSTALL_PREFIX}/include/faabric/mpi/mpi.h \
            ${CMAKE_INSTALL_PREFIX}/include/mpi.h   \
        )"
    )
endif()
