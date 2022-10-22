cmake_minimum_required(VERSION 3.14.0)
project(DSTest_Map VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_DefaultConstructor)
target_link_libraries(${PROJECT_NAME}_DefaultConstructor PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_DefaultConstructor PRIVATE ${SRC_ROOT}/DefaultConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_DefaultConstructor COMMAND ${PROJECT_NAME}_DefaultConstructor)

add_executable(${PROJECT_NAME}_CopyConstructor)
target_link_libraries(${PROJECT_NAME}_CopyConstructor PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_CopyConstructor PRIVATE ${SRC_ROOT}/CopyConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_CopyConstructor COMMAND ${PROJECT_NAME}_CopyConstructor)

add_executable(${PROJECT_NAME}_MoveConstructor)
target_link_libraries(${PROJECT_NAME}_MoveConstructor PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_MoveConstructor PRIVATE ${SRC_ROOT}/MoveConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_MoveConstructor COMMAND ${PROJECT_NAME}_MoveConstructor)

add_executable(${PROJECT_NAME}_AttachBalance)
target_include_directories(${PROJECT_NAME}_AttachBalance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
target_link_libraries(${PROJECT_NAME}_AttachBalance PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_AttachBalance PRIVATE ${SRC_ROOT}/AttachBalance.cpp)
add_test(NAME ${PROJECT_NAME}_AttachBalance COMMAND ${PROJECT_NAME}_AttachBalance)

add_executable(${PROJECT_NAME}_DetachBalance)
target_include_directories(${PROJECT_NAME}_DetachBalance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
target_link_libraries(${PROJECT_NAME}_DetachBalance PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_DetachBalance PRIVATE ${SRC_ROOT}/DetachBalance.cpp)
add_test(NAME ${PROJECT_NAME}_DetachBalance COMMAND ${PROJECT_NAME}_DetachBalance)
