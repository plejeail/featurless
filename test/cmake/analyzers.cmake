set(ENABLE_CPPCHECK TRUE)
set(ENABLE_CLANG_TIDY TRUE)
set(ENABLE_INCLUDE_WHAT_YOU_USE TRUE)

function(set_target_cppcheck DIRECTORY)
    if(ENABLE_ANALYZERS AND ENABLE_CPPCHECK)
        find_program(CPPCHECK cppcheck)
        if(CPPCHECK)
            message(STATUS "cppcheck found and enabled")
            set(CPPCHECK
                ${CPPCHECK}
                ${DIRECTORY}
                --suppress=missingInclude
                --suppress=unmatchedSuppression
                --suppress=uninitMemberVar
                --suppress=noConstructor
                --suppress=useStlAlgorithm
                #--file-filter=sources/*
                --enable=all
                --inline-suppr
                --inconclusive)
            execute_process(COMMAND ${CPPCHECK})
        else()
            message(STATUS "cppcheck not found.")
        endif()
    endif()
endfunction()
if(ENABLE_ANALYZERS)
    if(ENABLE_CLANG_TIDY)
        find_program(CLANGTIDY clang-tidy)
        if(CLANGTIDY)
            message(STATUS "clang-tidy found and enabled")
            set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
        else()
            message(STATUS "clang-tidy executable not found")
        endif()
    endif()
    if(ENABLE_INCLUDE_WHAT_YOU_USE)
        find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
        if(INCLUDE_WHAT_YOU_USE)
            message(STATUS "include-what-you-use found and enabled")
            set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
        else()
            message(STATUS "include-what-you-use executable not found")
        endif()
    endif()
    if(NOT CPPCHECK AND NOT CLANGTIDY AND NOT INCLUDE_WHAT_YOU_USE)
        message(SEND_ERROR "Static analyzer enabled, but none of them was found.")
    endif()
endif()

# Workaround to have clang-tidy precompiled header with GCC
function(target_precompiled_headers_tidy TARGET_NAME HEADER)
    set(PREFIX_HEADER "${CMAKE_CURRENT_LIST_DIR}/${HEADER}")
    set(CLANG_PCH_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${TARGET_NAME}.dir/cmake_pch.hxx.pch")
    get_property(CLANG_PCH_COMPILE_OPTIONS TARGET ${TARGET_NAME} PROPERTY COMPILE_OPTIONS)
    get_property(CLANG_PCH_INCLUDE_DIRECTORIES TARGET ${TARGET_NAME} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
    set(CLANG_PCH_INCLUDE_STRING "")
    foreach(dir ${CLANG_PCH_INCLUDE_DIRECTORIES})
        # Generate system includes to suppress warnings for third-party headers
        set(CLANG_PCH_INCLUDE_STRING ${CLANG_PCH_INCLUDE_STRING} -isystem ${dir})
    endforeach()
    add_custom_command(
        OUTPUT "${CLANG_PCH_OUTPUT}"
        COMMAND clang++ ${CLANG_PCH_COMPILE_OPTIONS} -std=c++20 ${CLANG_PCH_INCLUDE_STRING} -x c++-header ${PREFIX_HEADER} -Xclang -emit-pch -o ${CLANG_PCH_OUTPUT}
        COMMENT "Creating Clang precompiled header for clang-tidy."
    )
    add_custom_target(clang-pch ALL
        DEPENDS ${CLANG_PCH_OUTPUT}
    )
    add_dependencies(${TARGET_NAME} clang-pch)  # Create "fake" precompiled header for clang-tidy
endfunction()