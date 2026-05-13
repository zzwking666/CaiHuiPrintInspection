cmake_minimum_required(VERSION 3.17)

project(conMon VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/private/include
	${CMAKE_CURRENT_SOURCE_DIR}/private/src
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} ${GENERATED_UI_HEADERS})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Widgets)

if(CONFIG_TEST)
    add_subdirectory(./test)
	add_subdirectory(./testQt)
endif()


