file(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_LIST_DIR} test_*.cpp)

add_executable(
        tests
        main.cpp
        ${TEST_FILES}
)

target_link_libraries(tests emulator faasm faabric_test_utils)

add_test(NAME tests COMMAND "tests/test/tests")
