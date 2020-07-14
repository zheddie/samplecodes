cmake_minimum_required(VERSION 3.14)
project(Hello VERSION 1.0)
add_executable(Hello hello.cpp)
configure_file(Hello.h.in Hello.h)
target_include_directories(Hello PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
