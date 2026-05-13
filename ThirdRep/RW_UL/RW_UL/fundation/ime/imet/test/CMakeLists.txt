project(imet_test VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE imet)

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${IMET_PRIVATE_PATH}/TensorRT8.6/include 
    PRIVATE ${IME_UTILTY_ROOTPATH}/private/include
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TensorRT_dlls}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
