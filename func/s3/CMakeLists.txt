set(FAASM_USER s3)

function(s3_func exec_name dir_path)
    faasm_func(${exec_name} ${dir_path})
    set(ALL_DEMO_FUNCS ${ALL_DEMO_FUNCS} ${exec_name} PARENT_SCOPE)
endfunction(s3_func)

s3_func(list_buckets list_buckets.cpp)

# Custom target to group all the demo functions
add_custom_target(s3_all_funcs DEPENDS ${ALL_DEMO_FUNCS})
