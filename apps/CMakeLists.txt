add_executable(lab1 lab1.cpp)
target_link_libraries(lab1 PRIVATE compiler_lib)
set_project_warnings(lab1)

add_executable(lab2 lab2.cpp)
target_link_libraries(lab2 PRIVATE compiler_lib)
set_project_warnings(lab2)

add_executable(lab3 lab3.cpp)
target_link_libraries(lab3 PRIVATE compiler_lib)
set_project_warnings(lab3)
