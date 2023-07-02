cmake_minimum_required(VERSION 3.14.0)
project(${PROJECT_NAME}_Vector VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_DefaultConstructor)
target_link_libraries(${PROJECT_NAME}_DefaultConstructor PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_DefaultConstructor PRIVATE ${SRC_ROOT}/DefaultConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_DefaultConstructor COMMAND ${PROJECT_NAME}_DefaultConstructor)
