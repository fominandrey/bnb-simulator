cmake_minimum_required(VERSION 3.0)

set(PROJECT pools)

project(${PROJECT})

set(SOURCES
    ../abstract_task_pool.cpp ../abstract_task_map.cpp
    ../task_box.cpp ../task_map.cpp ../task_hash_map.cpp
)

add_library(${PROJECT} ${SOURCES})
