add_executable(example_test example_test.cpp)
target_link_libraries(example_test PRIVATE GTest::gtest_main)
set_project_warnings(example_test)

add_executable(string_source_test ${CMAKE_SOURCE_DIR}/src/modules/Source/implementations/StringSource/StringSource.class.test.cpp)
target_link_libraries(string_source_test PRIVATE GTest::gtest_main compiler_lib)
set_project_warnings(string_source_test)

add_executable(file_source_test ${CMAKE_SOURCE_DIR}/src/modules/Source/implementations/FileSource/FileSource.class.test.cpp)
target_link_libraries(file_source_test PRIVATE GTest::gtest_main compiler_lib)
set_project_warnings(file_source_test)

include(GoogleTest)
gtest_discover_tests(example_test)
gtest_discover_tests(string_source_test)
gtest_discover_tests(file_source_test)
