set(TARGET urt_api)
project(${TARGET})

cmake_minimum_required(VERSION 2.4)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.39.0 REQUIRED COMPONENTS
    python
)

find_package(PythonLibs)

find_package(Qt4)

include(${QT_USE_FILE})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})



set(SOURCES
    urt_api.cpp
)

set(HEADERS
    urt_api.h
)


add_library(${TARGET} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${QT_LIBRARIES} common clsyslog)

 
