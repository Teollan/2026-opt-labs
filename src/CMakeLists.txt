file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")

add_library(compiler_lib ${LIB_SOURCES})

target_include_directories(compiler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/StringSource
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/FileSource
    ${CMAKE_CURRENT_SOURCE_DIR}/Tokenizer
    ${CMAKE_CURRENT_SOURCE_DIR}/SymbolStore
    ${CMAKE_CURRENT_SOURCE_DIR}/CharacterAttributes
)

set_project_warnings(compiler_lib)
