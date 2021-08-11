set(FAASM_USER ffmpeg)

if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    # FFmpeg build
    set(FFMPEG_INCLUDE_DIRS
        ${CMAKE_SYSROOT}/include/libavcodec
        ${CMAKE_SYSROOT}/include/libavformat
    )
    set(FFMPEG_LIBS
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavcodec.a
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavformat.a
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libavutil.a
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libswscale.a
        ${CMAKE_SYSROOT}/lib/wasm32-wasi/libswresample.a
    )

    faasm_func(check check.cpp)

    target_include_directories(check PUBLIC ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(check ${FFMPEG_LIBS})

    add_custom_target(ffmpeg_all_funcs DEPENDS check)
else ()
    set(STATUS "FFmpeg must be built to WASM")
endif()
