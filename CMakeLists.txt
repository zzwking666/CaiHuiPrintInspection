cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(MaiLiDingZiSolution VERSION 0.1 LANGUAGES CXX CUDA)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/iniRW_UL.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeCacheIni.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ComplierIni.cmake)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/install/${CMAKE_BUILD_TYPE})

add_subdirectory(ThirdRep)
add_subdirectory(MaiLiDingZi)
