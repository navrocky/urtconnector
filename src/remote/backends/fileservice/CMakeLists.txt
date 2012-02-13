set(TARGET fileservice)

find_package(Qt4 REQUIRED QtCore)
include(${QT_USE_FILE})

set(HEADERS
    fileservice.h
)


set(MOC_HEADERS
    fileservice.h
)

set(SOURCES
    fileservice.cpp
)

set(IFACES
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} common)



