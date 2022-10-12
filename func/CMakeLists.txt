cmake_minimum_required(VERSION 3.8.0)
project(faasm-func)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FAASM_NATIVE_INSTALL "/usr/local/faasm/native")

if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    message(STATUS "Detected wasm build (sysroot=${CMAKE_SYSROOT})")

    SET(CMAKE_EXE_LINKER_FLAGS "${FAASM_EXE_LINKER_FLAGS} \
        -Xlinker --export=_faasm_zygote \
        -Xlinker --export=__heap_base \
        -Xlinker --export=__data_end \
        -Xlinker --export=__wasm_call_ctors \
        -Xlinker --max-memory=4294901760 \
        -Wl,--stack-first \
        -Wl,-z,stack-size=4194304 -Wl, \
    ")

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

        # These are the linker flags required for functions that will also do
        # dynamic linking.
        # We need to export all symbols to make them available to the
        # dynamically loaded modules
        set_target_properties(${func_name} PROPERTIES
                LINK_FLAGS "-Xlinker --export-all -Xlinker --no-gc-sections"
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
