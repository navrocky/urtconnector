project(clsyslog)

cmake_minimum_required(VERSION 2.4)

find_package( Boost COMPONENTS thread filesystem)
include_directories(${Boost_INCLUDE_DIRS})

set(TARGET clsyslog)
set(SOURCES
    common.h
    message.h
    message.cpp
    filter.h
    filter.cpp
    output.h
    output.cpp
    manager.h
    manager.cpp
    syslog.h
    syslog.cpp
    output_common.h
    output_common.cpp
    output_stream.h
    output_stream.cpp
    output_file.h
    output_file.cpp
    output_syslog.h
    output_syslog.cpp
)

add_library(${TARGET} STATIC ${SOURCES})

target_link_libraries(${TARGET} ${Boost_LIBRARIES} clexcept)

 
