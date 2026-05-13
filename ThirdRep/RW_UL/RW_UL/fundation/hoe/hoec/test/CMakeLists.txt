cmake_minimum_required(VERSION 3.17)

project(hoec_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(GTest CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# google test library
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}  PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

##test library
target_link_libraries(${PROJECT_NAME} PRIVATE hoec)
target_include_directories(${PROJECT_NAME} 
	PRIVATE ${MVS_PATH}/Includes
	PUBLIC ${HOEC_PRIVATE_PATH}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 检查是否为Windows操作系统
if(WIN32)
    message(STATUS "当前操作系统为Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "当前为64位系统")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${MVS_PATH}/Libraries/windows64/MvCameraControl.lib)
        # 在这里添加特定于Win64的设置
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        message(STATUS "当前为32位系统")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${MVS_PATH}/Libraries/windows32/MvCameraControl.lib)
        # 在这里添加特定于Win32的设置
    endif()
else()
    message(STATUS "当前操作系统不是Windows")
    # 在这里添加特定于非Windows的设置
endif()

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})