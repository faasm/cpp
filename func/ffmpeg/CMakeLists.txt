set(FAASM_USER ffmpeg)

if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
    # FFmpeg build
    set(FFMPEG_INCLUDE_DIRS
        ${CMAKE_SYSROOT}/include/libavcodec
        ${CMAKE_SYSROOT}/include/libavformat
    )
    set(FFMPEG_LIBS
        ${CMAKE_SYSROOT}/lib/libavcodec.a
        ${CMAKE_SYSROOT}/lib/libavformat.a
        ${CMAKE_SYSROOT}/lib/libavutil.a
        ${CMAKE_SYSROOT}/lib/libswscale.a
        ${CMAKE_SYSROOT}/lib/libswresample.a
    )

    faasm_func(check check.cpp)

    target_include_directories(check PUBLIC ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(check ${FFMPEG_LIBS})
else ()
    set(STATUS "FFmpeg must be built to WASM")
endif()
