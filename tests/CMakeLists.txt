add_executable(example_test example_test.cpp)
target_link_libraries(example_test PRIVATE GTest::gtest_main)
set_project_warnings(example_test)

add_executable(string_source_test ${CMAKE_SOURCE_DIR}/src/modules/Source/implementations/StringSource/StringSource.class.test.cpp)
target_link_libraries(string_source_test PRIVATE GTest::gtest_main compiler_lib)
set_project_warnings(string_source_test)

add_executable(file_source_test ${CMAKE_SOURCE_DIR}/src/modules/Source/implementations/FileSource/FileSource.class.test.cpp)
target_link_libraries(file_source_test PRIVATE GTest::gtest_main compiler_lib)
set_project_warnings(file_source_test)

add_executable(symbol_store_test ${CMAKE_SOURCE_DIR}/src/modules/SymbolStore/SymbolStore.class.test.cpp)
target_link_libraries(symbol_store_test PRIVATE GTest::gtest_main compiler_lib)
set_project_warnings(symbol_store_test)

add_executable(tokenizer_test ${CMAKE_SOURCE_DIR}/src/modules/Tokenizer/Tokenizer.class.test.cpp)
target_link_libraries(tokenizer_test PRIVATE GTest::gtest_main compiler_lib)
target_compile_definitions(tokenizer_test PRIVATE TEST_FILES_DIR="${CMAKE_SOURCE_DIR}/tests/files")
set_project_warnings(tokenizer_test)

include(GoogleTest)
gtest_discover_tests(example_test)
gtest_discover_tests(string_source_test)
gtest_discover_tests(file_source_test)
gtest_discover_tests(symbol_store_test)
gtest_discover_tests(tokenizer_test)
