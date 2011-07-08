set(TARGET cloud)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#include_directories(${CMAKE_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#find_package( Boost 1.39.0 REQUIRED COMPONENTS
#    program_options
#    thread
#    date_time
#    filesystem
#    system
#)
#include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

set(HEADERS
    main_form.h
)

set(MOC_HEADERS
    main_form.h
)

set(SOURCES
    main_form.cpp
    main.cpp
)

set(IFACES
    main_form.ui
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES} ${Boost_LIBRARIES}
)
