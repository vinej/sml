cmake_minimum_required (VERSION 3.13.2)
project (SML)

set (SML_VERSION_MAJOR 0)
set (SML_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#        "${PROJECT_SOURCE_DIR}/SMLConfig.h.in"
#        "${PROJECT_BINARY_DIR}/SMLConfig.h"
#)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${CMAKE_SOURCE_DIR}/include" "/usr/lib")
link_directories(/usr/local/lib)

add_executable(SML main.c stack.c command.c complex.c constants.c file.c function.c kexpr.c  matrix.c property.c str.c vector.c)

target_link_libraries (SML gsl gslcblas m)