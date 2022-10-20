# Set where the library has been built
if (FAASM_BUILD_TYPE STREQUAL "wasm")
    set(TF_LIB ${CMAKE_SYSROOT}/lib/wasm32-wasi/libtensorflow-lite.a)
else()
    message(FATAL_ERROR "Tensorflow can only be built for WebAssembly")
endif()

set(TF_HEADERS
    ${CMAKE_SYSROOT}/include/tensorflow
    ${CMAKE_SYSROOT}/include/tensorflow/tensorflow/lite/tools/make/downloads/absl
    ${CMAKE_SYSROOT}/include/tensorflow/tensorflow/lite/tools/make/downloads/flatbuffers/include
)

add_library(inference_utils
    bitmap_helpers.cpp
    get_top_n.h
    get_top_n_impl.h
    inference.h
)

target_include_directories(inference_utils PUBLIC ${TF_HEADERS})

faasm_func(inference inference.cpp)

target_include_directories(inference PUBLIC ${TF_HEADERS})
target_link_libraries(inference faasm ${TF_LIB} inference_utils)

add_custom_target(tless_all_funcs DEPENDS inference)
