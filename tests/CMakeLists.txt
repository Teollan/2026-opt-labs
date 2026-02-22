include(GoogleTest)

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.test.cpp")

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main compiler_lib)
    set_project_warnings(${TEST_NAME})
    gtest_discover_tests(${TEST_NAME})
endforeach()