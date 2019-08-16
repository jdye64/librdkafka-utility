cmake_minimum_required(VERSION 3.14)
project(kafka_utility)

set(CMAKE_CXX_STANDARD 14)

#set(LIBRDKAFKA_LIB "$ENV{CONDA_PREFIX}/lib/librdkafka.so")
#set(LIBRDKAFKA++_LIB "$ENV{CONDA_PREFIX}/lib/librdkafka++.so")
#set(LIBRDKAFKA_INCLUDE_DIR "$ENV{CONDA_PREFIX}/include/librdkafka")

set(LIBRDKAFKA_LIB "/home/jdyer/miniconda3/envs/kafka_dev/lib/librdkafka.so")
set(LIBRDKAFKA++_LIB "/home/jdyer/miniconda3/envs/kafka_dev/lib/librdkafka++.so")
set(LIBRDKAFKA_INCLUDE_DIR "/home/jdyer/miniconda3/envs/kafka_dev/include/librdkafka")

message("LIBRDKAFKA LIB -> ${LIBRDKAFKA_LIB}")
message("LIBRDKAFKA++ LIB -> ${LIBRDKAFKA++_LIB}")
message("LIBRDKAFKA_INCLUDE_DIR -> ${LIBRDKAFKA_INCLUDE_DIR}")

add_executable(kafka_utility main.cpp)
include_directories(${LIBRDKAFKA_INCLUDE_DIR})
target_link_libraries(kafka_utility ${LIBRDKAFKA_LIB} ${LIBRDKAFKA++_LIB})

install(TARGETS kafka_utility
        DESTINATION lib)