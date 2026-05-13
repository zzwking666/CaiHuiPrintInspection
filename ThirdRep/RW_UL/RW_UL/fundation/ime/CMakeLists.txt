set(UTILTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/utilty/include )

set(IME_UTILTY_ROOTPATH ${CMAKE_CURRENT_SOURCE_DIR}/utilty/)

FIND_SOURCES_AND_HEADERS(UTILTY_SRC_LIST
	${IME_UTILTY_ROOTPATH}/include 
	${IME_UTILTY_ROOTPATH}/src
)

FIND_SOURCES_AND_HEADERS(UTILTY_SRC_LIST_PRIVATE
	${IME_UTILTY_ROOTPATH}/private/include 
	${IME_UTILTY_ROOTPATH}/private/src
)

add_subdirectory(imet)
add_subdirectory(imeo)
add_subdirectory(ime)

if(CONFIG_TEST)
    add_subdirectory(utilty)
endif()
