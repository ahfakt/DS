cmake_minimum_required(VERSION 3.14.0)
project(DSTest_List VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_Holder)
target_include_directories(${PROJECT_NAME}_Holder PRIVATE inc)
target_link_libraries(${PROJECT_NAME}_Holder PRIVATE DS)
target_sources(${PROJECT_NAME}_Holder PRIVATE ${SRC_ROOT}/Holder.cpp)
add_test(NAME ${PROJECT_NAME}_Holder COMMAND ${PROJECT_NAME}_Holder)

add_executable(${PROJECT_NAME}_DefaultConstructor)
target_include_directories(${PROJECT_NAME}_DefaultConstructor PRIVATE inc)
target_link_libraries(${PROJECT_NAME}_DefaultConstructor PRIVATE DS)
target_sources(${PROJECT_NAME}_DefaultConstructor PRIVATE ${SRC_ROOT}/DefaultConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_DefaultConstructor COMMAND ${PROJECT_NAME}_DefaultConstructor)
