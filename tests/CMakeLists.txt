add_executable(example_test example_test.cpp)
target_link_libraries(example_test PRIVATE GTest::gtest_main)
set_project_warnings(example_test)

include(GoogleTest)
gtest_discover_tests(example_test)
