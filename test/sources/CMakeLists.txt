add_library(featurless_test STATIC )

set(HEADER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/featurless)
target_sources(featurless_test PRIVATE "tests.cpp" ${HEADER_INCLUDE_DIR}/test.h)
target_include_directories(featurless_test 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(featurless::ftest ALIAS featurless_test)
