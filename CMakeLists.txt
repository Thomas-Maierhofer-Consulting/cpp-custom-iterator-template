cmake_minimum_required(VERSION 3.16)
project(tmc-custom-iterator-template CXX)
# project(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package( Threads )

enable_testing()
find_package(GTest)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE 
    include/tmc/foundation/custom-iterator-template.hpp
    include/tmc/foundation/custom-iterator-template-helper.hpp
    )

target_include_directories(${PROJECT_NAME} INTERFACE 
    include
    )

add_executable(${PROJECT_NAME}-test)
target_sources(${PROJECT_NAME}-test PRIVATE 
    test/test-main.cpp
    test/custom-iterator-template-test.cpp
    )

target_link_libraries(${PROJECT_NAME}-test
    ${PROJECT_NAME} 
    ${GTEST_LIBRARIES}
    Threads::Threads
    )