set(FAASM_USER threads)

if (CMAKE_SYSTEM_NAME STREQUAL "WASI")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xlinker --shared-memory")
endif ()

function(thread_func exec_name dir_path)
    faasm_func(${exec_name} ${dir_path})
    set(ALL_THREADS_FUNCS ${ALL_THREADS_FUNCS} ${exec_name} PARENT_SCOPE)
endfunction(thread_func)

thread_func(threads_local threads_local.cpp)
thread_func(threads_check threads_check.cpp)
thread_func(threads_dist threads_dist.cpp)
thread_func(threads_memory threads_memory.cpp)

# Custom target for building all functions
add_custom_target(threads_all_funcs DEPENDS ${ALL_THREADS_FUNCS})
