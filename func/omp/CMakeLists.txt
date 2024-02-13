set(FAASM_USER omp)

if (CMAKE_SYSTEM_NAME STREQUAL "WASI")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xlinker --shared-memory")
else ()
    find_package(OpenMP REQUIRED)
    include_directories(${OPENMP_INCLUDE_DIR} ${LLVM_INCLUDE_DIRS}/openmp)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

function(omp_func exec_name dir_path)
    faasm_func(${exec_name} ${dir_path})

    target_link_libraries(${exec_name} faasmp)

    if (NOT CMAKE_SYSTEM_NAME STREQUAL "WASI")
        target_link_libraries(${exec_name} pthread)
    endif ()

    # Add to list of all funcs
    set(ALL_OMP_FUNCS ${ALL_OMP_FUNCS} ${exec_name} PARENT_SCOPE)
endfunction(omp_func)


# Single host parallelism only
omp_func(barrier_stress barrier_stress.cpp)
omp_func(complex_reduce complex_reduce.cpp)
omp_func(custom_reduce custom_reduce.cpp)
omp_func(default_shared default_shared.cpp)
omp_func(for_static_schedule for_static_schedule.cpp)
omp_func(header_api_support header_api_support.cpp)
omp_func(hellomp hellomp.cpp)
omp_func(inspect_reduction inspect_reduction.cpp)
omp_func(mem_stress mem_stress.cpp)
omp_func(nested_parallel nested_parallel.cpp)
omp_func(omp_checks omp_checks.cpp)
omp_func(pi_faasm pi_faasm.cpp)
omp_func(pi_native pi_native.cpp)
omp_func(reduction_integral reduction_integral.cpp)
omp_func(repeated_reduce repeated_reduce.cpp)
omp_func(setting_num_threads setting_num_threads.cpp)
omp_func(simple_atomic simple_atomic.cpp)
omp_func(simple_barrier simple_barrier.cpp)
omp_func(simple_critical simple_critical.cpp)
omp_func(simple_for simple_for.cpp)
omp_func(simple_master simple_master.cpp)
omp_func(simple_reduce simple_reduce.cpp)
omp_func(simple_single simple_single.cpp)
omp_func(single_thread_reduce single_thread_reduce.cpp)
omp_func(wtime wtime.cpp)

# Custom target for building all functions
add_custom_target(omp_all_funcs DEPENDS ${ALL_OMP_FUNCS})
