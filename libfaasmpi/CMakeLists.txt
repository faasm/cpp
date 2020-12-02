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

add_library(faasmpi STATIC ${LIB_FILES})
set_target_properties(faasmpi PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

# Install Faabric MPI library
set(FAABRIC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../third-party/faabric/src/mpi)
set(FAABRIC_BINARY_DIR ${FAABRIC_SOURCE_DIR}/build)

add_subdirectory(${FAABRIC_SOURCE_DIR} ${FAABRIC_BINARY_DIR})
add_dependencies(faasmpi faabricmpi)
install(FILES ${FAABRIC_BINARY_DIR}/libfaabricmpi.a
        DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
)

if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(STATUS "Libfaasmpi WebAssembly build")

    install(TARGETS faasmpi
            ARCHIVE DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            LIBRARY DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            PUBLIC_HEADER DESTINATION ${CMAKE_SYSROOT}/include/faasmpi
    )

    # Add imports file
    install(FILES faasmpi.imports
            DESTINATION ${CMAKE_SYSROOT}/lib/wasm32-wasi
            RENAME libfaasmpi.imports
    )

    # Symlink library as libmpi
    install(CODE "execute_process( \
        COMMAND ${CMAKE_COMMAND} -E create_symlink \
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libfaabricmpi.a \
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libmpi.a   \
        )"
    )
else()
    message(STATUS "Libfaasmpi native build")

    install(TARGETS faasmpi
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include/faasmpi
            )
endif()

# Symlink header (common to wasm and native builds)
# WARNING - this may overwrite the existing mpi.h on the system 
install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/include/faabric/mpi/mpi.h \
    ${CMAKE_INSTALL_PREFIX}/include/mpi.h   \
    )"
)
