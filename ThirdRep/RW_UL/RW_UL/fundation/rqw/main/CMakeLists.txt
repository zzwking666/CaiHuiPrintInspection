cmake_minimum_required(VERSION 3.17)

project(rqw VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include/ui
	${CMAKE_CURRENT_SOURCE_DIR}/include/componet
	${CMAKE_CURRENT_SOURCE_DIR}/src/ui
	${CMAKE_CURRENT_SOURCE_DIR}/src/componet
)
file(GLOB UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui")

qt_wrap_ui(GENERATED_UI_HEADERS ${UI_FILES})

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} ${GENERATED_UI_HEADERS})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/componet
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} 
	PUBLIC dsl
	PRIVATE cla
	PUBLIC oso
)

if(CONFIG_TEST)
    #add_subdirectory(./test)
endif()
