cmake_minimum_required(VERSION 3.17)
project(DS VERSION 0.9 DESCRIPTION "")

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} INTERFACE)
add_subdirectory(${PROJECT_SOURCE_DIR}/../Stream ${PROJECT_BINARY_DIR}/../../Stream/${CMAKE_BUILD_TYPE})

target_link_libraries(${PROJECT_NAME} INTERFACE Stream)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_sources(${PROJECT_NAME} INTERFACE
		include/${PROJECT_NAME}/Digraph.h
		include/${PROJECT_NAME}/List.h
		include/${PROJECT_NAME}/Map.h
		include/${PROJECT_NAME}/Vector.h
		src/Container.hpp
		src/Digraph.hpp
		src/LNode.hpp
		src/ENode.hpp
		src/List.hpp
		src/Map.hpp
		src/MNode.hpp
		src/Vector.hpp
		src/VNode.hpp)
