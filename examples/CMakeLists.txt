# Include the native builds
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    /usr/local/faasm/native/include
    )
link_directories(/usr/local/faasm/native/lib)

# Here we create a dummy library to check linking with static/ shared objects.
# We must link directly to the installed libraries so that we replicate the
# behaviour of external projects.
if(BUILD_SHARED_LIBS)
    add_library(native_lib SHARED native_lib.cpp)
    target_link_libraries(native_lib 
        libfaasm.so 
        libfaabric.so 
        libemulator.so
        libprotobuf.so
        )
else()
    add_library(native_lib STATIC native_lib.cpp)
    target_link_libraries(native_lib 
        libfaasm.a 
        libfaabric.a 
        libemulator.a
        libprotobuf.a
        )
endif()

add_executable(native_exe native_exe.cpp)
target_link_libraries(native_exe native_lib)
