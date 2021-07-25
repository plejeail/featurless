add_library(${PROJECT_NAME} STATIC )

set(HEADER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/featurless)
target_sources(${PROJECT_NAME} PRIVATE log.cpp ${HEADER_INCLUDE_DIR}/log.h)
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (NOT MAIN_PROJECT)
    add_library(featurless::log ALIAS ${PROJECT_NAME})
endif()

