message(STATUS "Retrieve compatible Qt environments")

# 检查QT_ROOT环境变量并设置qt路径
if(DEFINED ENV{QT_ROOT})
    message("QT_ROOT：$ENV{QT_ROOT}")

    # 查找符合条件的Qt路径
    if(WIN32)
        # Windows 下查找 Qt 路径
        file(GLOB QT_PATHS "$ENV{QT_ROOT}/[56]*/msvc2019_64/")
    elseif(UNIX)
        # Linux 下查找 Qt 路径
        file(GLOB QT_PATHS "$ENV{QT_ROOT}/[56]*/gcc_64/")
    endif()
    if(QT_PATHS)
        list(GET QT_PATHS 0 QT_PATH) # 获取第一个匹配的路径
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${QT_PATH}") # 添加到CMAKE_PREFIX_PATH
        message("Found QT path: ${QT_PATH}")

        # 设置Qt平台插件路径
        set(ENV{QT_PLUGIN_PATH} "${QT_PATH}/plugins")
        message("Set QT_PLUGIN_PATH to ${QT_PATH}/plugins")
    else()
        message(FATAL_ERROR "No QT path found that meets the criteria (Qt major version should be 5 or 6)")
    endif()
else()
    message(FATAL_ERROR "QT_ROOT environment variable undefined")
    message("Please define QT_ROOT in the system (which should be the root directory of qt)")
endif()

if(DEFINED ENV{HALCONROOT})
    message("HALCONROOT: $ENV{HALCONROOT}")

    set(HALCON_BIN_PATH "$ENV{HALCONROOT}/bin")
    set(HALCON_INCLUDE_PATH "$ENV{HALCONROOT}/include")
    set(HALCON_LIB_PATH "$ENV{HALCONROOT}/lib")
    set(HALCON_ROOT $ENV{HALCONROOT})
    if(EXISTS ${HALCON_BIN_PATH} AND EXISTS ${HALCON_INCLUDE_PATH} AND EXISTS ${HALCON_LIB_PATH})
        
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${HALCON_ROOT}")
        message("Found HALCON path: $ENV{HALCONROOT}")
        message("HALCON bin path: ${HALCON_BIN_PATH}")
        message("HALCON include path: ${HALCON_INCLUDE_PATH}")
        message("HALCON lib path: ${HALCON_LIB_PATH}")
        set(INSTALL_HALCON "true")
    else()
        set(INSTALL_HALCON "false") 
        message("HALCONROOT path is invalid. Required subdirectories 'bin', 'include', and 'lib' are missing.")
    endif()
else()
    set(INSTALL_HALCON "false") 
    message("HALCONROOT environment variable undefined")
    message("Please define HALCONROOT in the system (which should be the root directory of HALCON)")
endif()


if(DEFINED ENV{ZFKJ_DEVELOPMENT})
    set(ZFKJ_DEVELOPMENT_ROOT $ENV{ZFKJ_DEVELOPMENT})
else()
    set(ZFKJ_DEVELOPMENT_ROOT "D:/zfkjDevelopment")
endif()

# 检查CUDA是否可用
find_package(CUDA REQUIRED)
set(CUDA_IS_12 false)
set(CUDA_IS_11 false)

if(CUDA_FOUND)
    message(STATUS "CUDA found: Version ${CUDA_VERSION}")
    if(CUDA_VERSION VERSION_GREATER_EQUAL "12.0" AND CUDA_VERSION VERSION_LESS "13.0")
        set(TensorFindPath ${ZFKJ_DEVELOPMENT_ROOT}/ThirdLibrary/TensorRTs/cuda12/10.12)
        set(CUDA_IS_12 true)
        message(STATUS "CUDA version is 12.x, TensorFindPath is ${TensorFindPath}")
    elseif(CUDA_VERSION VERSION_GREATER_EQUAL "11.0" AND CUDA_VERSION VERSION_LESS "12.0" AND  NOT BUILD_CUDA113_WITH_TENSOR86)
        set(TensorFindPath ${ZFKJ_DEVELOPMENT_ROOT}/ThirdLibrary/TensorRTs/cuda11/10.12)
        set(CUDA_IS_11 true)
        message(STATUS "CUDA version is 11.x, TensorFindPath is ${TensorFindPath}")
    elseif(CUDA_VERSION VERSION_GREATER_EQUAL "11.0" AND CUDA_VERSION VERSION_LESS "12.0" AND BUILD_CUDA113_WITH_TENSOR86)
        set(TensorFindPath ${ZFKJ_DEVELOPMENT_ROOT}/ThirdLibrary/TensorRTs/cuda11/8.6)
        set(CUDA_IS_11 true)
        message(STATUS "CUDA version is 11.x, TensorFindPath is ${TensorFindPath}")
    else()
        message(FATAL_ERROR "CUDA version is not 11.x or 12.x, skipping specific logic")
    endif()
else()
    message(FATAL_ERROR "CUDA not found. Please ensure CUDA is installed and available in your environment.")
endif()