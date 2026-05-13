project(imeo VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(CUDAToolkit REQUIRED)

set(IMEO_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private)

FIND_SOURCES_AND_HEADERS(SRC_LIST
	${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IMEO_PRIVATE_PATH}/include 
	${IMEO_PRIVATE_PATH}/src
)

add_library(${PROJECT_NAME} ${SRC_LIST} ${UTILTY_SRC_LIST} ${UTILTY_SRC_LIST_PRIVATE})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${IMEO_PRIVATE_PATH}/include 
    PUBLIC ${UTILTY_INCLUDES}
    PRIVATE ${IME_UTILTY_ROOTPATH}/private/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC  ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} 
	PUBLIC  lgm
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ${OpenCV_INCLUDE_DIRS}
	PUBLIC ${OnnxRuntime_include}
    PUBLIC ${CUDAToolkit_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    CUDA::cudart               # CUDA Runtime Library
    CUDA::cublas               # cuBLAS Library
    CUDA::cufft                # cuFFT Library
    CUDA::curand               # cuRAND Library
    CUDA::cusolver             # cuSolver Library
    CUDA::cusparse             # cuSPARSE Library
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OnnxRuntime_libs})


if(CONFIG_TEST)
    add_subdirectory(./test)
endif()
