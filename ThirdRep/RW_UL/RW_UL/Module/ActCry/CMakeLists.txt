cmake_minimum_required(VERSION 3.17)

project(actCry VERSION 0.1 LANGUAGES CXX)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} ${GENERATED_UI_HEADERS})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
	PRIVATE hoei
	PRIVATE cla
)


if(CONFIG_TEST)
    add_subdirectory(./test)
endif()
