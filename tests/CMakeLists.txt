file(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_LIST_DIR} test_*.cpp)

add_executable(
        tests
        main.cpp
        matrix_utils.cpp
        matrix_utils.h
        utils.cpp
        ${TEST_FILES}
)

target_include_directories(tests PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(tests PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../libfaasm")

target_link_libraries(tests emulator faasm faabric_test_utils)

add_test(NAME tests COMMAND "tests/test/tests")
