cmake_minimum_required(VERSION 3.17)

project(actCry_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(GTest CONFIG REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main)

#rw库

target_link_libraries(${PROJECT_NAME} PRIVATE actCry cla)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# GoogleTest integration
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
