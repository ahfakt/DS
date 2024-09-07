cmake_minimum_required(VERSION 3.20.0)
project(DS VERSION 0.7 DESCRIPTION "Data Structures Template Library")


set(CMAKE_CXX_STANDARD 20)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	#add_compile_definitions(__GCC_CONSTRUCTIVE_SIZE=64 __GCC_DESTRUCTIVE_SIZE=64) #target_compile_definitions
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	#add_compile_options(--param destructive-interference-size=64) #target_compile_options
	#add_compile_definitions(__GCC_CONSTRUCTIVE_SIZE=64 __GCC_DESTRUCTIVE_SIZE=64) #target_compile_definitions
endif()


file(GLOB INC ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}/*.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${INC})
target_include_directories(${PROJECT_NAME} INTERFACE inc)


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
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.cfg ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg @ONLY)
		add_custom_target(${PROJECT_NAME}Doc
			WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
			VERBATIM)
	endif (DOXYGEN_FOUND)
endif()
