set(TARGET python)
project(${TARGET})

cmake_minimum_required(VERSION 2.4)

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.39.0 REQUIRED COMPONENTS
    python
)

find_package(PythonLibs)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(api)

set(SOURCES
    python_api.cpp
    python_engine.cpp
)

set(HEADERS
    python_api.h
    python_engine.h
)

set(MOC_HEADERS
    python_engine.h
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})


add_library(${TARGET} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

install(FILES main.py DESTINATION share/urtconnector)
