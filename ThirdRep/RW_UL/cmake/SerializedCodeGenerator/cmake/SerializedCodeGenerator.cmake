# oso_wrap_oso.cmake
function(oso_wrap_oso OUTVAR)
    # OUTVAR: 变量名，后面参数为 oso 文件列表
    set(options)
    set(oneValueArgs OUTPUT_DIR)
    set(multiValueArgs)
    cmake_parse_arguments(OSO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(OSO_OUTPUT_DIR)
        set(gen_dir "${OSO_OUTPUT_DIR}")
    else()
        set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/oso_gen")
    endif()
    file(MAKE_DIRECTORY "${gen_dir}")

    set(_generated_headers)
    foreach(oso_file ${OSO_UNPARSED_ARGUMENTS})
        get_filename_component(oso_abs "${oso_file}" ABSOLUTE)
        get_filename_component(oso_name "${oso_file}" NAME_WE)

        # 先用python脚本分析类名和命名空间，得到输出hpp名
        execute_process(
            COMMAND ${Python3_EXECUTABLE} "${SerializedCodeGeneratorCmake}/SerializedCodeGenerator.py" "${oso_abs}" "${gen_dir}"
            RESULT_VARIABLE RET
        )
        if(NOT RET EQUAL 0)
            message(FATAL_ERROR "Failed to generate header from ${oso_file}")
        endif()

        # 这里假设输出文件名为 cdm_ClassName.hpp 或 ClassName.hpp
        file(READ "${oso_abs}" OSO_CONTENT)
        string(REGEX MATCH "class[ \t]+([A-Za-z0-9_]+)" _ ${OSO_CONTENT})
        set(class_name "${CMAKE_MATCH_1}")

        # 检查是否有二级命名空间
        string(REGEX MATCH "namespace[ \t]+([A-Za-z0-9_]+)[^}]*namespace[ \t]+([A-Za-z0-9_]+)" _ ${OSO_CONTENT})
        if(CMAKE_MATCH_2)
            set(hpp_name "${CMAKE_MATCH_2}_${class_name}.hpp")
        else()
            set(hpp_name "${class_name}.hpp")
        endif()
        set(hpp_path "${gen_dir}/${hpp_name}")

        add_custom_command(
            OUTPUT "${hpp_path}"
            COMMAND ${Python3_EXECUTABLE} "${SerializedCodeGeneratorCmake}/SerializedCodeGenerator.py" "${oso_abs}" "${gen_dir}"
            DEPENDS "${oso_abs}" "${SerializedCodeGeneratorCmake}/SerializedCodeGenerator.py"
            COMMENT "Generating ${hpp_path} from ${oso_file}"
        )
        list(APPEND _generated_headers "${hpp_path}")
    endforeach()
    set(${OUTVAR} ${_generated_headers} PARENT_SCOPE)
endfunction()