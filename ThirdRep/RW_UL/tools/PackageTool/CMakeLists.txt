cmake_minimum_required(VERSION 3.17)

project(PackageTool VERSION 0.1 LANGUAGES CXX)

# 你的工作区要求 C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)   # 直接把 .ui 当普通源文件列入即可
set(CMAKE_AUTORCC ON)

# 找 Qt6；若 CMake 找不到，设置环境变量 Qt6_DIR 指向 .../lib/cmake/Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

#（可选）如果以后需要 OpenCV 再打开
# find_package(OpenCV REQUIRED)
# message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# 列出源文件。也可以改成 GLOB，但显式列出更可控
set(SRC_FILES
    main.cpp
    PackageTool.cpp
)

set(HEADER_FILES
    PackageTool.h
)

set(UI_FILES
    PackageTool.ui
)

set(QRC_FILES
    PackageTool.qrc
)

# 生成可执行文件
qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${QRC_FILES}
)

# 没有额外子目录头文件时这一步可省；如果以后加 include 目录再补
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Concurrent
        # ${OpenCV_LIBS}    # 如果启用 OpenCV
)

# (可选) 统一二进制输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 自动处理 Qt 6 收尾
qt_finalize_executable(${PROJECT_NAME})

# Windows 部署 Qt 依赖（自动推导 windeployqt）
if(WIN32)
    # 利用 Qt6::qmake 位置反推出 windeployqt
    get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    if(_qmake_exe)
        get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
        set(_windeployqt "${_qt_bin_dir}/windeployqt.exe")
        if(EXISTS "${_windeployqt}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${_windeployqt}" --no-compiler-runtime --no-opengl-sw "$<TARGET_FILE:${PROJECT_NAME}>"
                COMMENT "Running windeployqt to bundle Qt runtime"
            )
        else()
            message(WARNING "windeployqt 未找到：${_windeployqt}")
        endif()
    endif()
endif()

set(SEVEN_Z_PATH "C:/Program Files/7-Zip/7z.exe" CACHE FILEPATH "Path to 7z executable")