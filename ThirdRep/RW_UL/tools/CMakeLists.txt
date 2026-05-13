if(BUILD_WITH_CUDA)
	add_subdirectory(AutomaticAnnotation)
	add_subdirectory(ModelConverter)
endif()

if(CONFIG_TEST)
    add_subdirectory(demo)
	add_subdirectory(demoNoQt)
endif()

add_subdirectory(PackageTool)
add_subdirectory(XmlMerge)
add_subdirectory(DiskStorageManager)
add_subdirectory(DataSetAnalysis)


if(CONFIG_TEST)
    add_subdirectory(LicenseIssuance)
endif()