cmake_minimum_required(VERSION 3.0)
project(emulator)

set(PUBLIC_HEADERS
        faasm/emulator.h
        faasm/emulator_api.h
        )

set(LIB_FILES emulator.cpp)

if(BUILD_SHARED_LIBS)
    add_library(emulator SHARED "${LIB_FILES}")
else()
    add_library(emulator STATIC "${LIB_FILES}")
endif()

target_include_directories(emulator PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(emulator 
    faasm 
    faabric
    faabric_test_utils
    )

set_target_properties(emulator PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(TARGETS emulator
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/faasm
        )
