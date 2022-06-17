set(FAASM_USER ffmpeg)

function(ffmpeg_func exec_name dir_path is_c)
    if (CMAKE_SYSTEM_NAME STREQUAL "Wasm")
        # FFmpeg build
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

        if (is_c)
            faasm_func_c(${exec_name} ${dir_path})
        else ()
            faasm_func(${exec_name} ${dir_path})
        endif ()

        target_include_directories(${exec_name} PUBLIC ${FFMPEG_INCLUDE_DIRS})
        target_link_libraries(${exec_name} ${FFMPEG_LIBS})

        set(ALL_MPEG_FUNCS ${ALL_MPEG_FUNCS} ${exec_name} PARENT_SCOPE)
    else ()
        set(STATUS "FFmpeg functions must be built to WASM")
    endif()
endfunction(ffmpeg_func)

ffmpeg_func(check check.cpp 0)
ffmpeg_func(encode encode.c 1)
ffmpeg_func(filter image_filtering.cpp 0)
ffmpeg_func(filter_end hello.cpp 0)
ffmpeg_func(recognition image_recognition.cpp 0)
ffmpeg_func(recognition_end hello.cpp 0)
# TODO - remove this functions eventually
ffmpeg_func(gen_chunks generate_video_chunks.c 1)
ffmpeg_func(raw raw.cpp 0)
ffmpeg_func(raw_chained raw_chained.cpp 0)

add_custom_target(ffmpeg_all_funcs DEPENDS ${ALL_MPEG_FUNCS})
# TODO -remove this target
add_custom_target(tless_funcs DEPENDS encode filter filter_end recognition recognition_end)
