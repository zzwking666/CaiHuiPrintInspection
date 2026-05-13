cmake_minimum_required(VERSION 3.17)

project(oso VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pugixml CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

set(OSO_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${OSO_PRIVATE_PATH}/include
	${OSO_PRIVATE_PATH}/src
)

group_sources_and_headers(
	"Private"
	${OSO_PRIVATE_PATH}/include
	${OSO_PRIVATE_PATH}/src
)

group_interface_sources_and_headers(
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME}  STATIC ${SRC_LIST})
target_link_libraries(${PROJECT_NAME}  
	PRIVATE pugixml::pugixml
	PRIVATE JsonCpp::JsonCpp
	PRIVATE unofficial::sqlite3::sqlite3
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private/include
)

if(CONFIG_TEST)
    add_subdirectory(./test)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH}/${PROJECT_NAME}/include/)
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${INSTALL_PATH}/${PROJECT_NAME}/lib/)



