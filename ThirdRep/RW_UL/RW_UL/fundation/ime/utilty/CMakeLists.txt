project(ime_utilty_test VERSION 0.1 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)


find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} ${UTILTY_SRC_LIST} ${UTILTY_SRC_LIST_PRIVATE} "./test/main.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC  ${OpenCV_LIBS})

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${IME_UTILTY_ROOTPATH}/include
    PRIVATE ${IME_UTILTY_ROOTPATH}/private/include)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TensorRT_dlls}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
