set(RW_UL_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ThirdRep/RW_UL/cmake)

include(${RW_UL_CMAKE_PATH}/CMakeCahceIni.cmake)
include(${RW_UL_CMAKE_PATH}/func.cmake)
include(${RW_UL_CMAKE_PATH}/EnvironmetIni.cmake)
include(${RW_UL_CMAKE_PATH}/PackageFind.cmake)
include(${RW_UL_CMAKE_PATH}/ComplierIni.cmake)

set(SerializedCodeGeneratorCmake ${RW_UL_CMAKE_PATH}/SerializedCodeGenerator)
list(APPEND CMAKE_MODULE_PATH "${RW_UL_CMAKE_PATH}/SerializedCodeGenerator/cmake")
include(SerializedCodeGenerator)