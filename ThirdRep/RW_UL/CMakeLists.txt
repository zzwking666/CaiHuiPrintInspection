cmake_minimum_required(VERSION 3.10)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeCahceIni.cmake)

# 检查当前 CMakeLists 是否为根 CMakeLists
#如果是根 CMakeLists，则设置不构建测试
set(CONFIG_TEST false)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(CONFIG_TEST true)
	
	set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

	find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(SerializedCodeGeneratorCmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SerializedCodeGenerator)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SerializedCodeGenerator/cmake")
    include(SerializedCodeGenerator)
    include(./cmake/func.cmake)

    set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/install/${CMAKE_BUILD_TYPE})
endif()

if(BUILD_WITH_CUDA)
	project(RW_UL VERSION 0.1 LANGUAGES CXX CUDA)
else()
	project(RW_UL VERSION 0.1 LANGUAGES CXX)
endif()

include(./cmake/EnvironmetIni.cmake)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	include(./cmake/ComplierIni.cmake)
	include(./cmake/PackageFind.cmake)
endif()

find_package(gtest REQUIRED CONFIG)

add_subdirectory(RW_UL)
add_subdirectory(tools)

enable_testing()