################################################################################
#                            PROJECT MAIN CMAKE FILE                           #
#                                                                              #
################################################################################
cmake_minimum_required(VERSION 3.20)


# Project configuration.........................................................
project(FEATURLESS_LOGGER VERSION 1.0.0)
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

if (MAIN_PROJECT AND BUILD_TESTS)
    message("-- featurless::log is the main project. compiling 'tests'. Require fmt::format.")
    add_subdirectory("lib/fmt")
    add_subdirectory("test")
endif()