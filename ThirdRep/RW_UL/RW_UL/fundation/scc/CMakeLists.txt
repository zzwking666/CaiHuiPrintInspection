project(scc VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME}  ${SRC_LIST})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${ZMotionRutime_include}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ZMotionRutime_libs})

if(CONFIG_TEST)
    add_subdirectory(./test)
endif()
