
set(TF_DIR /code/cpp/third-party/tensorflow/)
set(TF_LITE_DIR ${TF_DIR}/tensorflow/lite)
set(TF_LITE_DOWNLOAD_DIR ${TF_LITE_DIR}/tools/make/downloads)
set(TF_BUILD_DIR ${TF_LITE_DIR}/tools/make/gen/)



# Set where the library has been built
if (FAASM_BUILD_TYPE STREQUAL "wasm")
    set(TF_LIB ${TF_BUILD_DIR}/wasm32-unknown-wasi_x86_64/lib/libtensorflow-lite.a)
else()
    set(TF_LIB ${TF_BUILD_DIR}/linux_x86_64/lib/libtensorflow-lite.a)
endif()

faasm_func(minimal minimal.cpp)

# Include TF headers and 3rd party
include_directories(minimal PUBLIC
        ${TF_DIR}
        ${TF_LITE_DIR}
        ${TF_LITE_DOWNLOAD_DIR}
        ${TF_LITE_DOWNLOAD_DIR}/eigen
        ${TF_LITE_DOWNLOAD_DIR}/absl
        ${TF_LITE_DOWNLOAD_DIR}/gemmlowp
        ${TF_LITE_DOWNLOAD_DIR}/neon_2_sse
        ${TF_LITE_DOWNLOAD_DIR}/farmhash/src
        ${TF_LITE_DOWNLOAD_DIR}/flatbuffers/include
)

#add_executable(minimal minimal.cpp)
target_link_libraries(minimal faasm ${TF_LIB})
add_custom_target(tf_all_funcs DEPENDS minimal)
