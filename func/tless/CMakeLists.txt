set(FAASM_USER demo)

function(tless_func exec_name dir_path)
    faasm_func(${exec_name} ${dir_path})
    set(ALL_TLESS_FUNCS ${ALL_TLESS_FUNCS} ${exec_name} PARENT_SCOPE)
endfunction(demo_func)

tless_func(noop noop.cpp)
tless_func(noop_chain noop_chain.cpp)

# Custom target to group all the demo functions
add_custom_target(demo_all_funcs DEPENDS ${ALL_DEMO_FUNCS})
