function(set_target_sanitizer target_name)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

    if(ENABLE_COVERAGE)
      target_compile_options(${target_name} INTERFACE --coverage -O0 -g)
      target_link_libraries(${target_name} INTERFACE --coverage)
    endif()

    set(SANITIZERS "")
    if(ENABLE_SANITIZER_MEMORY AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        list(APPEND SANITIZERS "memory")
    else()
        if(ENABLE_SANITIZER_THREAD)
            list(APPEND SANITIZERS "thread")
        else()
            list(APPEND SANITIZERS "address")
            list(APPEND SANITIZERS "leak")
        endif()
    endif()
    list(APPEND SANITIZERS "undefined")

    list(
      JOIN
      SANITIZERS
      ","
      LIST_OF_SANITIZERS)
  endif()

  if(LIST_OF_SANITIZERS AND ENABLE_SANITIZERS)
      target_compile_options(${target_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
      target_link_options(${target_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
  endif()
endfunction()
