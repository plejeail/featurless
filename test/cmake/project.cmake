# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo")
endif()

set(CMAKE_CROSSCOMPILING True)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
endif()

# unique debug flag for everyone
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DEBUG)
endif()

# set log level
string(TOLOWER $CACHE{LOGGER_LEVEL} logger_level)
if(NOT ${logger_level} STREQUAL "none")
    message(STATUS "log level: ${logger_level}.")
    if(${logger_level} STREQUAL "debug")
        add_compile_definitions(LOG_LEVEL=4)
    elseif(${logger_level} STREQUAL "info")
        add_compile_definitions(LOG_LEVEL=3)
    elseif(${logger_level} STREQUAL "warning")
        add_compile_definitions(LOG_LEVEL=2)
    elseif(${logger_level} STREQUAL "error")
        add_compile_definitions(LOG_LEVEL=1)
    else()
        message(WARNING "Unknown log level '${logger_level}'.")
    endif()
endif()

# For vs-code intellisense
find_program(PROGRAM_GTAGS NAMES gtags)
if(PROGRAM_GTAGS)
    message(STATUS "Gtags regenerated.")
    execute_process(COMMAND ${PROGRAM_GTAGS} -C ${CMAKE_SOURCE_DIR})
endif()