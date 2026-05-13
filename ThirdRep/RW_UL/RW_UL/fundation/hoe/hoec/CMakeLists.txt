cmake_minimum_required(VERSION 3.17)

project(hoec VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOEC_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private)
set(MVS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ThirdLibrary/MVS)

find_package(OpenCV REQUIRED)

FIND_SOURCES_AND_HEADERS(SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${HOEC_PRIVATE_PATH}/include
    ${HOEC_PRIVATE_PATH}/src
)

group_sources_and_headers(
	"Private"
	${HOEC_PRIVATE_PATH}/include
	${HOEC_PRIVATE_PATH}/src
)

group_interface_sources_and_headers(
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
target_include_directories(${PROJECT_NAME} 
    PRIVATE ${MVS_PATH}/Includes
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

# 检查是否为Windows操作系统
if(WIN32)
    #message(STATUS "当前操作系统为Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        #message(STATUS "当前为64位系统")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${MVS_PATH}/Libraries/windows64/MvCameraControl.lib)
        # 在这里添加特定于Win64的设置
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        #message(STATUS "当前为32位系统")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${MVS_PATH}/Libraries/windows32/MvCameraControl.lib)
        # 在这里添加特定于Win32的设置
    endif()
else()
    #message(STATUS "当前操作系统不是Windows")
    # 在这里添加特定于非Windows的设置
endif()


if(CONFIG_TEST)
    add_subdirectory(./test)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_PATH}/${PROJECT_NAME}/include/)
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${INSTALL_PATH}/${PROJECT_NAME}/lib/)





