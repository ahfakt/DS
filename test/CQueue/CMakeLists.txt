cmake_minimum_required(VERSION 3.20.0)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME Class)
project(${PROJECT_NAME}_${Class} VERSION 0.1 DESCRIPTION "")


set(INC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}_MultipleProducer)
target_link_libraries(${PROJECT_NAME}_MultipleProducer PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_MultipleProducer PRIVATE ${SRC_ROOT}/MultipleProducer.cpp)
add_test(NAME ${PROJECT_NAME}_MultipleProducer COMMAND ${PROJECT_NAME}_MultipleProducer 10 1000000 5)

add_executable(${PROJECT_NAME}_MultipleConsumer)
target_link_libraries(${PROJECT_NAME}_MultipleConsumer PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_MultipleConsumer PRIVATE ${SRC_ROOT}/MultipleConsumer.cpp)
add_test(NAME ${PROJECT_NAME}_MultipleConsumer COMMAND ${PROJECT_NAME}_MultipleConsumer 10 1000000 5)

add_executable(${PROJECT_NAME}_MultipleProducerMultipleConsumer)
target_link_libraries(${PROJECT_NAME}_MultipleProducerMultipleConsumer PRIVATE Stream DS)
target_sources(${PROJECT_NAME}_MultipleProducerMultipleConsumer PRIVATE ${SRC_ROOT}/MultipleProducerMultipleConsumer.cpp)
add_test(NAME ${PROJECT_NAME}_MultipleProducerSingleConsumer COMMAND ${PROJECT_NAME}_MultipleProducerMultipleConsumer 10 1000000 5 1)
add_test(NAME ${PROJECT_NAME}_SingleProducerMultipleConsumer COMMAND ${PROJECT_NAME}_MultipleProducerMultipleConsumer 10 1000000 1 5)
add_test(NAME ${PROJECT_NAME}_MultipleProducerMultipleConsumer COMMAND ${PROJECT_NAME}_MultipleProducerMultipleConsumer 10 1000000 3 4)

