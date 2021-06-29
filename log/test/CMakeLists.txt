if(BUILD_TESTS)
    add_executable(PROJECTTESTS test_main.cpp)
    target_link_libraries(PROJECTTESTS PRIVATE ${PROJECT_NAME})
endif()
