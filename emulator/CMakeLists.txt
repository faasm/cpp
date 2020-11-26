cmake_minimum_required(VERSION 3.0)
project(faasm-emulator)

set(PUBLIC_HEADERS
        faasm/emulator.h
        faasm/emulator_api.h
        )

set(LIB_FILES emulator.cpp)

add_library(emulator STATIC "${LIB_FILES}")

target_include_directories(emulator PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(emulator faasm faabric)

set_target_properties(emulator PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(TARGETS emulator
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/faasm
        )
