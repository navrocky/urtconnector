set(TARGET geoip)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    GeoIP_impl.h
    geoip.h
)

set(SOURCES
    GeoIP_impl.c
    geoip.cpp
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

if(WIN32)
    target_link_libraries( ${TARGET} ${QT_LIBRARIES} ws2_32)
else(WIN32)
    target_link_libraries( ${TARGET} ${QT_LIBRARIES} )
endif(WIN32)
