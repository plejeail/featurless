################################################################################
#                            PROJECT MAIN CMAKE FILE                           #
#                                                                              #
################################################################################
cmake_minimum_required(VERSION 3.20)


# Project configuration.........................................................
project(FEATURLESS_LOGGER VERSION 1.0.0)

option(BUILD_TESTS "Build tests executable" ON)

option(ENABLE_CACHE "Enable cache if available" OFF)
option(ENABLE_SANITIZER "Enable sanitizers" ON)
option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
option(ENABLE_ANALYZERS "Enable third-party found analysers" OFF)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

# Cmake includes ...............................................................
include("cmake/project.cmake")
include("cmake/cxx.cmake")
include("cmake/sanitizer.cmake")
include("cmake/analyzers.cmake")
include("cmake/cache.cmake")

# Project sources...............................................................
add_subdirectory("sources")
add_subdirectory("test")
