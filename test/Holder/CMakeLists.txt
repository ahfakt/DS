cmake_minimum_required(VERSION 3.20.0)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME Class)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)


add_executable(${PROJECT_NAME}_InitializableConstructor)
target_sources(${PROJECT_NAME}_InitializableConstructor PRIVATE ${SRC_ROOT}/InitializableConstructor.cpp)
target_include_directories(${PROJECT_NAME}_InitializableConstructor PRIVATE ../inc)
target_link_libraries(${PROJECT_NAME}_InitializableConstructor PRIVATE Stream DS)
add_test(NAME ${PROJECT_NAME}_InitializableConstructor COMMAND ${PROJECT_NAME}_InitializableConstructor)


add_executable(${PROJECT_NAME}_ExtractableConstructor)
target_sources(${PROJECT_NAME}_ExtractableConstructor PRIVATE ${SRC_ROOT}/ExtractableConstructor.cpp)
target_include_directories(${PROJECT_NAME}_ExtractableConstructor PRIVATE ../inc)
target_link_libraries(${PROJECT_NAME}_ExtractableConstructor PRIVATE Stream DS)
add_test(NAME ${PROJECT_NAME}_ExtractableConstructor COMMAND ${PROJECT_NAME}_ExtractableConstructor)
