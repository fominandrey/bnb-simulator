cmake_minimum_required(VERSION 3.0)

set(PROJECT communicators)

project(${PROJECT})

set(SOURCES ../communicator.cpp ../timer.cpp)

add_library(${PROJECT} ${SOURCES})
