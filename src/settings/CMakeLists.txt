set(TARGET settings)

find_package(Qt4 REQUIRED QtCore)
include(${QT_USE_FILE})

set(HEADERS
    settings.h
)

set(MOC_HEADERS
)

set(SOURCES
    settings.cpp
)

set(IFACES
)

set(RESOURCES
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )
