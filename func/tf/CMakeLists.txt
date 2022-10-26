if (FAASM_BUILD_TYPE STREQUAL "wasm")
    set(TF_LIB ${CMAKE_SYSROOT}/lib/wasm32-wasi/libtensorflow-lite.a)

    faasm_func(minimal minimal.cpp)

    set(TF_HEADERS
        ${CMAKE_SYSROOT}/include/tensorflow
        ${CMAKE_SYSROOT}/include/tensorflow/tensorflow/lite/tools/make/downloads/flatbuffers/include
    )

    target_include_directories(minimal PUBLIC ${TF_HEADERS})
    target_link_libraries(minimal faasm ${TF_LIB})
    add_custom_target(tf_all_funcs DEPENDS minimal)
else()
    message(STATUS "Tensorflow can only be built for WebAssembly")
    add_custom_target(tf_all_funcs)
endif()
