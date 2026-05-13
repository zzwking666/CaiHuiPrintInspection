# Halcon | Include: Halcon_include | Lib: Halcon_libs | 
if(WIN32)
    if(INSTALL_HALCON)
		find_all_libs(${HALCON_ROOT}/lib/x64-win64 Halcon_libs)
        set(Halcon_include ${HALCON_ROOT}/include)
    endif()
else()
endif()

# TensorRT | Include: TensorRT_include | Lib: TensorRT_libs | Dll: TensorRT_dlls |Exe: TensorRT_exes |
set(TensorRTPath ${TensorFindPath})
set(TensorRT_include ${TensorRTPath}/include)
find_all_libs(${TensorRTPath}/lib TensorRT_libs)
find_all_dlls(${TensorRTPath}/lib TensorRT_dlls)
find_all_exes(${TensorRTPath}/bin TensorRT_exes)

# OnnxRuntime | Include: OnnxRuntime_include | Lib: OnnxRuntime_libs | Dll: OnnxRuntime_dlls | 
set(OnnxRuntimePath ${ZFKJ_DEVELOPMENT_ROOT}/ThirdLibrary/onnxruntime-win-x64-gpu-1.21.0)
set(OnnxRuntime_include ${OnnxRuntimePath}/include)
find_all_libs(${OnnxRuntimePath}/lib OnnxRuntime_libs)
find_all_dlls(${OnnxRuntimePath}/lib OnnxRuntime_dlls)

# ZMotion | Include: ZMotionRutime_include | Lib: ZMotionRutime_libs | Dll: ZMotionRutime_dlls | 
set(ZMotionRutimePath ${ZFKJ_DEVELOPMENT_ROOT}/ThirdLibrary/ZMotion)
set(ZMotionRutime_include ${ZMotionRutimePath}/include)
find_all_libs(${ZMotionRutimePath}/lib ZMotionRutime_libs)
find_all_dlls(${ZMotionRutimePath}/dll ZMotionRutime_dlls)