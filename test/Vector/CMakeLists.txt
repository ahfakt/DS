cmake_minimum_required(VERSION 3.14.0)
project(DSTestVector VERSION 0.1 DESCRIPTION "")

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
