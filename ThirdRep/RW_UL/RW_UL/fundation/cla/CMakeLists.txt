cmake_minimum_required(VERSION 3.17)

project(cla VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cryptopp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/ActivationCodeModule
	${CMAKE_CURRENT_SOURCE_DIR}/include/EncrpyModule 
)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src/ActivationCodeModule
	${CMAKE_CURRENT_SOURCE_DIR}/include/EncrpyModule 
)

group_interface_sources_and_headers(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} )
target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ActivationCodeModule
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/EncrpyModule
)
target_link_libraries(${PROJECT_NAME} PRIVATE cryptopp::cryptopp)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)

if(CONFIG_TEST)
    add_subdirectory(./test)
endif()