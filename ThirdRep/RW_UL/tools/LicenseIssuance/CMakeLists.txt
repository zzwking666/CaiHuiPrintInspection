cmake_minimum_required(VERSION 3.17)

project(LicenseIssuance VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



FIND_SOURCES_AND_HEADERS(INCLUDE_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME} ./src/main.cpp)

target_include_directories(${PROJECT_NAME} 
	PRIVATE ${INCLUDE_LIST}
)

target_link_libraries(${PROJECT_NAME} PRIVATE actCry)










