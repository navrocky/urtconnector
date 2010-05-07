set(TARGET qtsingleapplication)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})

find_package(Qt4 REQUIRED QtGui QtCore QtNetwork)
include(${QT_USE_FILE})

set(URT_DATADIR ${CMAKE_INSTALL_PREFIX}/share/urtconnector/)

set(HEADERS
    qtlocalpeer.h
    qtlockedfile.h
    qtsingleapplication.h
    qtsinglecoreapplication.h
)

set(MOC_HEADERS
    qtlocalpeer.h
    qtsingleapplication.h
    qtsinglecoreapplication.h
)

set(SOURCES
    qtlocalpeer.cpp
    qtlockedfile.cpp
    qtsingleapplication.cpp
    qtsinglecoreapplication.cpp
)

set(IFACES
)

set(RESOURCES
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES})


