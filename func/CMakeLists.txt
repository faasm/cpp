cmake_minimum_required(VERSION 3.8.0)
project(faasm-func)

# -----------------------------------------
# Most of the variables used transitively used here like compiler or linker
# flags are set in the sysroot file (WasiToolchain.cmake) and are defined in
# the build environment file (faasmtools/build.py)
# -----------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FAASM_NATIVE_INSTALL $ENV{FAASM_NATIVE_INSTALL_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(STATUS "Detected wasm build (sysroot=$ENV{FAASM_WASM_SYSROOT})")
    SET(CMAKE_EXECUTABLE_SUFFIX ".wasm")
endif ()


if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    # ----------------------
    # Wasm build
    # ----------------------

    set(FAASM_FUNC_LIBS faasm c-printscan-long-double)

    function(faasm_func_c func_name file_name)
        add_executable(${func_name} ${file_name})
        target_link_libraries(${func_name} ${FAASM_FUNC_LIBS})
    endfunction(faasm_func_c)

    function(faasm_func func_name file_name)
        add_executable(${func_name} ${file_name})
        target_link_libraries(${func_name} ${FAASM_FUNC_LIBS})
    endfunction(faasm_func)

    function(faasm_dynamic_func func_name file_name)
        faasm_func(${func_name} ${file_name})

        # These are the compiler and linker flags required for functions that
        # will also do dynamic linking.  We need to export all symbols to make
        # them available to the dynamically loaded modules
        set_target_properties(${func_name} PROPERTIES
            COMPILE_FLAGS $ENV{FAASM_WASM_CFLAGS_SHARED}
            LINK_FLAGS $ENV{FAASM_WASM_SHARED_LINKER_FLAGS}
        )

    endfunction(faasm_dynamic_func)
else ()
    # ----------------------
    # Native build
    # ----------------------

    function(faasm_func_c func_name file_name)
        add_executable(${func_name} ${file_name})

        target_link_directories(${func_name} PRIVATE "${FAASM_NATIVE_INSTALL}/lib")
        target_include_directories(${func_name} PRIVATE "${FAASM_NATIVE_INSTALL}/include")

        target_link_libraries(${func_name} libfaasm.a)
    endfunction(faasm_func_c)

    function(faasm_func func_name file_name)
        faasm_func_c(${func_name} ${file_name})
    endfunction(faasm_func)

    function(faasm_dynamic_func func_name file_name)
        faasm_func(${func_name} ${file_name})
        target_link_libraries(${func_name} util)
    endfunction(faasm_dynamic_func)
endif ()

add_subdirectory(errors)
add_subdirectory(demo)
add_subdirectory(dynlink)
add_subdirectory(ffmpeg)
add_subdirectory(mpi)
add_subdirectory(omp)
add_subdirectory(tf)
