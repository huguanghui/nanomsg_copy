# cmake_minimum_required(VERSION 2.8.7)

# project(nanomsg-demo)

# find_package(nanomsg CONFIG REQUIRED)

add_executable(basic_demo basic_demo.c)
target_link_libraries(basic_demo nanomsg)
set_target_properties(basic_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                            ${PROJECT_BINARY_DIR})
