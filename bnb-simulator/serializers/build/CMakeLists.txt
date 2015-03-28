cmake_minimum_required(VERSION 3.0)

set(PROJECT serializers)

project(${PROJECT})

set(SOURCES ../serializer.cpp)

add_library(${PROJECT} ${SOURCES})
