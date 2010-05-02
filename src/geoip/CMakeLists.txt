set(TARGET geoip)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})

find_package( Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore QtGui QtXml)
include(${QT_USE_FILE})
# 
# configure_file(config.h.in config.h)

set(HEADERS
    GeoIP_impl.h
    geoip.h
)

set(MOC_HEADERS
)

set(SOURCES
    GeoIP_impl.c
    geoip.cpp
)

set(IFACES

)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
#qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
#qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)



