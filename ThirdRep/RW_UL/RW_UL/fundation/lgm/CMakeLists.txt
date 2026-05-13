project(lgm VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog/src
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog/private/include
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog/private/src
)

add_library(${PROJECT_NAME}  ${SRC_LIST} )

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/private/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
if(CONFIG_TEST)
    add_subdirectory(./test)
endif()


