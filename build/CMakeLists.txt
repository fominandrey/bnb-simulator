cmake_minimum_required(VERSION 3.0)

set(PROJECT bnbtest)

project(${PROJECT})

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

include_directories(../bnb-solver ../bnb-simulator)

set(SOURCES ../bnbtest.cpp ../apply_settings.cpp)

add_executable(${PROJECT} ${SOURCES})

set(LIBRARIES processes communicators serializers simulators pools)

foreach(LIB ${LIBRARIES})
    add_subdirectory(../bnb-simulator/${LIB}/build bin/${LIB})
endforeach()

set(JSON json)

add_library(${JSON} STATIC IMPORTED)

set_target_properties(${JSON} PROPERTIES
    IMPORTED_LOCATION ../bnb-solver/libjson/libjson.a
)

target_link_libraries(${PROJECT} ${LIBRARIES} ${JSON})
