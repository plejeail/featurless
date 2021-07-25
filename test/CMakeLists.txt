################################################################################
#                            PROJECT MAIN CMAKE FILE                           #
#                                                                              #
################################################################################
cmake_minimum_required(VERSION 3.20)


# Project configuration.........................................................
project(ftest VERSION 1.0.0 LANGUAGES CXX)

# Determine if ftest is built as a subproject (using add_subdirectory) or if it is the main project.
set(MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

option(BUILD_TESTS "Build tests executable" ON)

# Set standard feature used
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project sources...............................................................
add_subdirectory("sources")
add_subdirectory("test")
