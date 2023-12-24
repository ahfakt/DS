cmake_minimum_required(VERSION 3.20.0)
project(DS VERSION 0.7 DESCRIPTION "Data Structures Template Library")


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc)
file(GLOB INC ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}/*.hpp)
target_sources(${PROJECT_NAME} INTERFACE ${INC})


set(DEPENDENCIES DP Stream Format)
foreach (T IN LISTS DEPENDENCIES)
	if (NOT TARGET ${T})
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${T} ${CMAKE_CURRENT_BINARY_DIR}/../${T})
	endif ()
endforeach ()
target_link_libraries(${PROJECT_NAME} INTERFACE ${DEPENDENCIES})


if (CMAKE_BUILD_TYPE MATCHES Debug)
	enable_testing()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/Testing)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg @ONLY)
		add_custom_target(${PROJECT_NAME}Doc
			WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
			VERBATIM)
	endif (DOXYGEN_FOUND)
endif()
