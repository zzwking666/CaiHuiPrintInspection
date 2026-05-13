if(MSVC)
    # 对于 MSVC 编译器，启用 UTF-8 编码
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Mp>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 对于 GCC 或 Clang 编译器，设置源文件编码为 UTF-8
    #add_compile_options("-finput-charset=UTF-8")
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-finput-charset=UTF-8>)
endif()