if(BUILD_TESTS)
    add_executable(TESTS_FTESTS test_main.cpp)
    target_link_libraries(TESTS_FTESTS PRIVATE featurless::${PROJECT_NAME})
endif()
