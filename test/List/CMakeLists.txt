cmake_minimum_required(VERSION 3.20.0)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME Class)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_DefaultConstructor)
target_sources(${PROJECT_NAME}_DefaultConstructor PRIVATE ${SRC_ROOT}/DefaultConstructor.cpp)
target_link_libraries(${PROJECT_NAME}_DefaultConstructor PRIVATE Stream DS)
add_test(NAME ${PROJECT_NAME}_DefaultConstructor COMMAND ${PROJECT_NAME}_DefaultConstructor)

add_executable(${PROJECT_NAME}_StreamConstructor)
target_sources(${PROJECT_NAME}_StreamConstructor PRIVATE ${SRC_ROOT}/StreamConstructor.cpp)
target_include_directories(${PROJECT_NAME}_StreamConstructor PRIVATE ../inc)
target_link_libraries(${PROJECT_NAME}_StreamConstructor PRIVATE Stream DS)
add_test(NAME ${PROJECT_NAME}_StreamConstructor COMMAND ${PROJECT_NAME}_StreamConstructor)
