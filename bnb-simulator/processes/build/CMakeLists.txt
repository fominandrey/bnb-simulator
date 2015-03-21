cmake_minimum_required(VERSION 3.0)

set(PROJECT processes)

project(${PROJECT})

set(SOURCES ../process.cpp)

add_library(${PROJECT} ${SOURCES})
