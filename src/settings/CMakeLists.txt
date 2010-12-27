set(TARGET settings)

find_package(Qt4 REQUIRED QtCore)
include(${QT_USE_FILE})

set(HEADERS
    settings.h
)

set(MOC_HEADERS
    settings.h
)

set(SOURCES
    settings.cpp
)

set(IFACES
)

set(RESOURCES
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )
