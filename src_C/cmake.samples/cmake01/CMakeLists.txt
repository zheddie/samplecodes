project(Hello)
message( STATUS "zg.DEBUG.cp00,CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
set(SRC_LIST hello.c)
add_executable(hello ${SRC_LIST})
