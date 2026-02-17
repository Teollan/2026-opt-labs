file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/modules/**/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*\\.test\\.cpp$")

add_library(compiler_lib ${LIB_SOURCES})

target_include_directories(compiler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Symbol
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Source/implementations/StringSource
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Source/implementations/FileSource
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/Tokenizer
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/SymbolStore
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/CharacterAttributes
)

set_project_warnings(compiler_lib)
