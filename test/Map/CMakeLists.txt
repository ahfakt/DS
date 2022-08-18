cmake_minimum_required(VERSION 3.14.0)
project(DSTest_Map VERSION 0.1 DESCRIPTION "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_DefaultConstructor)
target_include_directories(${PROJECT_NAME}_DefaultConstructor PRIVATE include)
target_link_libraries(${PROJECT_NAME}_DefaultConstructor PRIVATE DS)
target_sources(${PROJECT_NAME}_DefaultConstructor
		PRIVATE
		${SRC_ROOT}/DefaultConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_DefaultConstructor COMMAND ${PROJECT_NAME}_DefaultConstructor)

add_executable(${PROJECT_NAME}_CopyConstructor)
target_include_directories(${PROJECT_NAME}_CopyConstructor PRIVATE include)
target_link_libraries(${PROJECT_NAME}_CopyConstructor PRIVATE DS)
target_sources(${PROJECT_NAME}_CopyConstructor
		PRIVATE
		${SRC_ROOT}/CopyConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_CopyConstructor COMMAND ${PROJECT_NAME}_CopyConstructor)

add_executable(${PROJECT_NAME}_MoveConstructor)
target_include_directories(${PROJECT_NAME}_MoveConstructor PRIVATE include)
target_link_libraries(${PROJECT_NAME}_MoveConstructor PRIVATE DS)
target_sources(${PROJECT_NAME}_MoveConstructor
		PRIVATE
		${SRC_ROOT}/MoveConstructor.cpp)
add_test(NAME ${PROJECT_NAME}_MoveConstructor COMMAND ${PROJECT_NAME}_MoveConstructor)

add_executable(${PROJECT_NAME}_AttachBalance)
target_include_directories(${PROJECT_NAME}_AttachBalance PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${PROJECT_NAME}_AttachBalance PRIVATE DS)
target_sources(${PROJECT_NAME}_AttachBalance
		PRIVATE
		${SRC_ROOT}/AttachBalance.cpp)
add_test(NAME ${PROJECT_NAME}_AttachBalance COMMAND ${PROJECT_NAME}_AttachBalance)

add_executable(${PROJECT_NAME}_DetachBalance)
target_include_directories(${PROJECT_NAME}_DetachBalance PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(${PROJECT_NAME}_DetachBalance PRIVATE DS)
target_sources(${PROJECT_NAME}_DetachBalance
		PRIVATE
		${SRC_ROOT}/DetachBalance.cpp)
add_test(NAME ${PROJECT_NAME}_DetachBalance COMMAND ${PROJECT_NAME}_DetachBalance)
