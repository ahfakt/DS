cmake_minimum_required(VERSION 3.14.0)
project(DS VERSION 0.7 DESCRIPTION "Data Structures Template Library")


set(DEPENDENCIES DP Stream StreamFormat)
set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_LIST
		${INC_ROOT}/${PROJECT_NAME}/Container.hpp
		${INC_ROOT}/${PROJECT_NAME}/Digraph.hpp
		${INC_ROOT}/${PROJECT_NAME}/List.hpp
		${INC_ROOT}/${PROJECT_NAME}/Map.hpp
		${INC_ROOT}/${PROJECT_NAME}/Set.hpp
		${INC_ROOT}/${PROJECT_NAME}/Vector.hpp)
set(SRC_LIST
		${SRC_ROOT}/${PROJECT_NAME}/Digraph.tpp
		${SRC_ROOT}/${PROJECT_NAME}/LNode.tpp
		${SRC_ROOT}/${PROJECT_NAME}/ENode.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Holder.tpp
		${SRC_ROOT}/${PROJECT_NAME}/List.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Map.tpp
		${SRC_ROOT}/${PROJECT_NAME}/MNode.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Set.tpp
		${SRC_ROOT}/${PROJECT_NAME}/SNode.tpp
		${SRC_ROOT}/${PROJECT_NAME}/TNode.tpp
		${SRC_ROOT}/${PROJECT_NAME}/Vector.tpp
		${SRC_ROOT}/${PROJECT_NAME}/VNode.tpp)


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE inc)
target_sources(${PROJECT_NAME} INTERFACE ${INC_LIST} PRIVATE ${SRC_LIST})

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
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
			VERBATIM)
	endif (DOXYGEN_FOUND)
endif()
