project(clexcept)

cmake_minimum_required(VERSION 2.4)

set(TARGET clexcept)
set(SOURCES
    base_exception.h
    base_exception.cpp
    error.h
    error.cpp
)

add_library(${TARGET} STATIC ${SOURCES})

 
