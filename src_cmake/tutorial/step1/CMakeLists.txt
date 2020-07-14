cmake_minimum_required(VERSION 3.10)

# set the project name
project(Tutorial VERSION 2.3)
configure_file(TutorialConfig.h.in TutorialConfig.h)
option(USE_MYMATH "Use tutorial provided math implementation" ON)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
  list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif()

# add the executable
add_executable(Tutorial Tutorial.cxx)
target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})



target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           ${EXTRA_INCLUDES}
                           )