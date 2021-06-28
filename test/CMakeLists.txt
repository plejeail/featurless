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

option(ENABLE_CACHE "Enable cache if available" ON)
option(ENABLE_SANITIZER "Enable sanitizers" OFF)
option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
option(ENABLE_ANALYZERS "Enable third-party found analysers (clang-tidy/cppcheck/include-what-you-use)" OFF)  
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Cmake includes ...............................................................
include("cmake/project.cmake")
include("cmake/cxx.cmake")
include("cmake/sanitizer.cmake")
include("cmake/analyzers.cmake")
include("cmake/cache.cmake")

# Project sources...............................................................
add_subdirectory("sources")
add_subdirectory("test")
