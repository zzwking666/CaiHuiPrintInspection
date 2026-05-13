cmake_minimum_required(VERSION 3.17)

project(demoNoQt VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(libmodbus CONFIG REQUIRED)

FIND_SOURCES_AND_HEADERS(INCLUDE_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB_RECURSE CUDA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu)

add_executable(${PROJECT_NAME} 
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cu
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE serial
	PRIVATE modbus
)

#target_include_directories(${PROJECT_NAME}
#    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
#    PRIVATE $ENV{CUDA_PATH}/include
#)
#
## 添加 CUDA 库目录（如需链接 CUDA 库时使用）
#target_link_directories(${PROJECT_NAME}
#    PRIVATE $ENV{CUDA_PATH}/lib/x64
#)













