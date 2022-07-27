cmake_minimum_required(VERSION 3.14.0)
project(DS VERSION 0.7 DESCRIPTION "")

set(CMAKE_CXX_STANDARD 20)


set(DEPENDENCIES DP Stream StreamFormat)
set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_LIST
		${INC_ROOT}/${PROJECT_NAME}/Container.h
		${INC_ROOT}/${PROJECT_NAME}/Digraph.h
		${INC_ROOT}/${PROJECT_NAME}/List.h
		${INC_ROOT}/${PROJECT_NAME}/Map.h
		${INC_ROOT}/${PROJECT_NAME}/Set.h
		${INC_ROOT}/${PROJECT_NAME}/Vector.h)
set(SRC_LIST
		${SRC_ROOT}/Digraph.hpp
		${SRC_ROOT}/LNode.hpp
		${SRC_ROOT}/ENode.hpp
		${SRC_ROOT}/Holder.hpp
		${SRC_ROOT}/List.hpp
		${SRC_ROOT}/Map.hpp
		${SRC_ROOT}/MNode.hpp
		${SRC_ROOT}/Set.hpp
		${SRC_ROOT}/SNode.hpp
		${SRC_ROOT}/TNode.hpp
		${SRC_ROOT}/Vector.hpp
		${SRC_ROOT}/VNode.hpp)

add_library(${PROJECT_NAME}OBJ INTERFACE)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}OBJ INTERFACE include)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_sources(${PROJECT_NAME}OBJ INTERFACE ${INC_LIST} PRIVATE ${SRC_LIST})
target_sources(${PROJECT_NAME} INTERFACE ${INC_LIST} PRIVATE ${SRC_LIST})

foreach (T IN LISTS DEPENDENCIES)
	if (NOT TARGET ${T})
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${T} ${CMAKE_CURRENT_BINARY_DIR}/../../../${T}/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})
	endif ()
endforeach ()
target_link_libraries(${PROJECT_NAME} INTERFACE ${DEPENDENCIES})
list(TRANSFORM DEPENDENCIES APPEND OBJ)
target_link_libraries(${PROJECT_NAME}OBJ INTERFACE ${DEPENDENCIES})


if (CMAKE_BUILD_TYPE MATCHES Debug)
	enable_testing()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/Testing)
endif()


if ((DEFINED DOC_ROOT) AND (NOT ${DOC_ROOT} STREQUAL ""))
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		set(DOXYGEN_IN_CFG ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.in.cfg)
		set(DOXYGEN_OUTPUT_DIR ${DOC_ROOT}/${PROJECT_NAME})
		set(DOXYGEN_CFG ${DOXYGEN_OUTPUT_DIR}/doxygen.cfg)

		configure_file(${DOXYGEN_IN_CFG} ${DOXYGEN_CFG} @ONLY)
		message("Doxygen configuration ${DOXYGEN_CFG} generated.")

		add_custom_target(${PROJECT_NAME}Doc
				COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CFG}
				WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR}
				COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
				VERBATIM)
	else (DOXYGEN_FOUND)
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif()
