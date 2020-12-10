cmake_minimum_required(VERSION 3.14)
project(Hello VERSION 1.0)
add_executable(Hello hello.cpp)
configure_file(Hello.h.in Hello.h)
target_include_directories(Hello PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
find_package(Python3 COMPONENTS Interpreter Development)

message("Python_FOUND:${Python3_FOUND}")
message("Python_VERSION:${Python3_VERSION}")
message("Python_Development_FOUND:${Python3_Development_FOUND}")
message("Python_LIBRARIES:${Python3_LIBRARIES}")
message("Python3_LIBRARIES:${Python3_LINK_OPTIONS}")

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
