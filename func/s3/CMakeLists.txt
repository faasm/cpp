set(FAASM_USER s3)

function(s3_func exec_name dir_path)
    faasm_func(${exec_name} ${dir_path})
    set(ALL_DEMO_FUNCS ${ALL_DEMO_FUNCS} ${exec_name} PARENT_SCOPE)
endfunction(s3_func)

s3_func(get_num_buckets get_num_buckets.cpp)
s3_func(list_buckets list_buckets.cpp)
s3_func(get_num_keys get_num_keys.cpp)
s3_func(list_keys list_keys.cpp)
s3_func(add_key_bytes add_key_bytes.cpp)
s3_func(get_key_bytes get_key_bytes.cpp)
# TODO: move elsewhere when it works
s3_func(wc_driver wc_driver.cpp)
s3_func(wc_mapper wc_mapper.cpp)

# Custom target to group all the demo functions
add_custom_target(s3_all_funcs DEPENDS ${ALL_DEMO_FUNCS})
