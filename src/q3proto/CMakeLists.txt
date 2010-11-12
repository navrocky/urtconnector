set(TARGET q3proto)

find_package(Qt4 REQUIRED QtCore QtNetwork)
include(${QT_USE_FILE})

set(HEADERS
)

set(MOC_HEADERS
)

set(SOURCES
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



