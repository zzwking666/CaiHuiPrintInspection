if(BUILD_WITH_CUDA)
	add_subdirectory(rqwime)
endif()

if(BUILD_WITH_MVS)
	if(BUILD_WITH_DS)
	add_subdirectory(rqwhoec)
	add_subdirectory(rqwm)
	endif()
endif()

if(BUILD_WITH_HALCON)
	if(WIN32)
		if(INSTALL_HALCON)
		add_subdirectory(rqwhalcon)
		endif()
	endif()
endif()

add_subdirectory(rqwscc)
#add_subdirectory(rqwmodbus)



