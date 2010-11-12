set(TARGET anticheat)

find_package(Qt4 REQUIRED QtGui )
include(${QT_USE_FILE})

set(HEADERS
    anticheat.h
)

set(MOC_HEADERS
    anticheat.h
)

set(SOURCES
    anticheat.cpp
)

set(IFACES
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



