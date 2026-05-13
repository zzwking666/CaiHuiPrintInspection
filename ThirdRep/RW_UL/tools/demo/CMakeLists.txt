cmake_minimum_required(VERSION 3.17)

project(demo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

file(GLOB UI_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui")
qt_wrap_ui(GENERATED_UI_HEADERS ${UI_FILES})

file(GLOB QRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/qrc/*.qrc")

include(SerializedCodeGenerator)
set(OSO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/osoFile/demoConfig.oso)
oso_wrap_oso(GENERATED_HEADERS ${OSO_FILES} OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/osoHpp)

FIND_SOURCES_AND_HEADERS(INCLUDE_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/osoHpp
)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

qt_add_executable(${PROJECT_NAME} 
	MANUAL_FINALIZATION
	${INCLUDE_LIST}
	${SRC_LIST}
	${GENERATED_UI_HEADERS}
	${QRC_FILES}
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/osoHpp
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Concurrent)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE rqw
	PRIVATE rqwime
	PRIVATE rqwhalcon
	PRIVATE rqwmodbus
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${QT_PATH}/bin/windeployqt.exe --no-compiler-runtime --no-opengl-sw $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Running windeployqt to package Qt dependencies"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TensorRT_exes} ${TensorRT_dlls} ${ZMotionRutime_dlls} ${OnnxRuntime_dlls}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE ${Halcon_include})

target_link_libraries(${PROJECT_NAME} 
	PRIVATE ${Halcon_libs})

# 安装可执行文件
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_PATH}/${PROJECT_NAME} # 可执行文件安装到 bin 目录
)

string(REPLACE "\\" "/" QT_PATH_FIXED ${QT_PATH})
install(CODE "
    execute_process(
        COMMAND ${QT_PATH_FIXED}/bin/windeployqt.exe --no-compiler-runtime --no-opengl-sw ${INSTALL_PATH}/${PROJECT_NAME}/${PROJECT_NAME}.exe
        WORKING_DIRECTORY ${INSTALL_PATH}/${PROJECT_NAME}
    )
    message(\"Running windeployqt during install phase\")
")









