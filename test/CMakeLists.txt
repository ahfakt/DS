cmake_minimum_required(VERSION 3.14.0)
project(DSTest VERSION 0.1 DESCRIPTION "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)


add_executable(${PROJECT_NAME}_Digraph_00)
target_include_directories(${PROJECT_NAME}_Digraph_00 PRIVATE include)
target_link_libraries(${PROJECT_NAME}_Digraph_00 PRIVATE DS)
target_sources(${PROJECT_NAME}_Digraph_00
		PRIVATE
		${SRC_ROOT}/Digraph_00.cpp)
add_test(NAME ${PROJECT_NAME}_Digraph_00 COMMAND ${PROJECT_NAME}_Digraph_00)


add_executable(${PROJECT_NAME}_List_00)
target_include_directories(${PROJECT_NAME}_List_00 PRIVATE include)
target_link_libraries(${PROJECT_NAME}_List_00 PRIVATE DS)
target_sources(${PROJECT_NAME}_List_00
		PRIVATE
		${SRC_ROOT}/List_00.cpp)
add_test(NAME ${PROJECT_NAME}_List_00 COMMAND ${PROJECT_NAME}_List_00)


add_executable(${PROJECT_NAME}_Map_00)
target_include_directories(${PROJECT_NAME}_Map_00 PRIVATE include)
target_link_libraries(${PROJECT_NAME}_Map_00 PRIVATE DS)
target_sources(${PROJECT_NAME}_Map_00
		PRIVATE
		${SRC_ROOT}/Map_00.cpp)
add_test(NAME ${PROJECT_NAME}_Map_00 COMMAND ${PROJECT_NAME}_Map_00)


add_executable(${PROJECT_NAME}_Set_00)
target_include_directories(${PROJECT_NAME}_Set_00 PRIVATE include)
target_link_libraries(${PROJECT_NAME}_Set_00 PRIVATE DS)
target_sources(${PROJECT_NAME}_Set_00
		PRIVATE
		${SRC_ROOT}/Set_00.cpp)
add_test(NAME ${PROJECT_NAME}_Set_00 COMMAND ${PROJECT_NAME}_Set_00)


add_executable(${PROJECT_NAME}_Vector_00)
target_include_directories(${PROJECT_NAME}_Vector_00 PRIVATE include)
target_link_libraries(${PROJECT_NAME}_Vector_00 PRIVATE DS)
target_sources(${PROJECT_NAME}_Vector_00
		PRIVATE
		${SRC_ROOT}/Vector_00.cpp)
add_test(NAME ${PROJECT_NAME}_Vector_00 COMMAND ${PROJECT_NAME}_Vector_00)