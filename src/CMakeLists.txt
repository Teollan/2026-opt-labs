file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.hpp")

set(INCLUDE_DIRS "")
foreach(HEADER ${LIB_HEADERS})
    get_filename_component(DIR ${HEADER} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

add_library(compiler_lib ${LIB_SOURCES})

target_include_directories(compiler_lib PUBLIC ${INCLUDE_DIRS})

set_project_warnings(compiler_lib)
