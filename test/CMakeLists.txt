cmake_minimum_required(VERSION 3.14.0)
project(DSTest VERSION 0.1 DESCRIPTION "")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Digraph ${CMAKE_CURRENT_BINARY_DIR}/Digraph)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/List ${CMAKE_CURRENT_BINARY_DIR}/List)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Map ${CMAKE_CURRENT_BINARY_DIR}/Map)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Set ${CMAKE_CURRENT_BINARY_DIR}/Set)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Vector ${CMAKE_CURRENT_BINARY_DIR}/Vector)
