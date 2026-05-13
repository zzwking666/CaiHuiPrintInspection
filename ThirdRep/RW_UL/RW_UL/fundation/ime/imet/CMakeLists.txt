project(imet VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)


find_package(OpenCV REQUIRED)
find_package(CUDAToolkit REQUIRED)

if(CUDA_IS_11)
	set(CMAKE_CUDA_ARCHITECTURES 61;75;80;86)
endif()

if(CUDA_IS_12)
	set(CMAKE_CUDA_ARCHITECTURES 75;80;86;89)
endif()


set(IMET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/)


if(BUILD_CUDA113_WITH_TENSOR86)
	FIND_SOURCES_AND_HEADERS(IMPLENT_LIST_INCLUDE
		${IMET_PRIVATE_PATH}/TensorRT8.6/include 
	)
	FIND_SOURCES_AND_HEADERS(IMPLENT_LIST_SRC
		${IMET_PRIVATE_PATH}/TensorRT8.6/src 
	)
	set(IMPLENT_INCLUDE ${IMET_PRIVATE_PATH}/TensorRT8.6/include )
else()
	FIND_SOURCES_AND_HEADERS(IMPLENT_LIST_INCLUDE
		${IMET_PRIVATE_PATH}/TensorRT10.12/include 
	)
	FIND_SOURCES_AND_HEADERS(IMPLENT_LIST_SRC
		${IMET_PRIVATE_PATH}/TensorRT10.12/src 
	)
	set(IMPLENT_INCLUDE ${IMET_PRIVATE_PATH}/TensorRT10.12/include )
endif()

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

FIND_CU_AND_HEADERS(CU_SRC_LIST
	${IMET_PRIVATE_PATH}/cuda/include
	${IMET_PRIVATE_PATH}/cuda/src
)

add_library(${PROJECT_NAME}  ${SRC_LIST} ${UTILTY_SRC_LIST} ${UTILTY_SRC_LIST_PRIVATE} ${CU_SRC_LIST} ${IMPLENT_LIST_INCLUDE} ${IMPLENT_LIST_SRC})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${IMPLENT_INCLUDE}
    PUBLIC ${UTILTY_INCLUDES}
    PRIVATE ${IME_UTILTY_ROOTPATH}/private/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC  ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} 
	PUBLIC  lgm
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${OpenCV_INCLUDE_DIRS}
	PUBLIC ${TensorRT_include}
    PUBLIC ${CUDAToolkit_INCLUDE_DIRS}
	PUBLIC ${IMET_PRIVATE_PATH}/cuda/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CUDA::cudart               # CUDA Runtime Library
    CUDA::cublas               # cuBLAS Library
    CUDA::cufft                # cuFFT Library
    CUDA::curand               # cuRAND Library
    CUDA::cusolver             # cuSolver Library
    CUDA::cusparse             # cuSPARSE Library
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${TensorRT_libs})

if(CONFIG_TEST)
    add_subdirectory(./test)
endif()