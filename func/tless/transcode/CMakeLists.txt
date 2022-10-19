set(FFMPEG_INCLUDE_DIRS
    ${CMAKE_SYSROOT}/include/libavcodec
    ${CMAKE_SYSROOT}/include/libavfilter
    ${CMAKE_SYSROOT}/include/libavformat
    ${CMAKE_SYSROOT}/include/libavutil
)

set(FFMPEG_LIBS
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavcodec.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavdevice.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavfilter.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavformat.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavutil.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libswscale.a
    ${CMAKE_SYSROOT}/lib/wasm32-wasi/libswresample.a
)

faasm_func(transcode transcode.cpp)

target_include_directories(transcode PUBLIC ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(transcode ${FFMPEG_LIBS})
