set(TARGET common)

#find_package(Qt4 REQUIRED QtCore )
#include(${QT_USE_FILE})

set(HEADERS
    scoped_tools.h
)

set(MOC_HEADERS
)

if (${QT_VERSION_MAJOR}.${QT_VERSION_MINOR} VERSION_GREATER 4.6)
set(MOC_HEADERS ${MOC_HEADERS}
    iconned_dock_style.h
)

endif()

set(SOURCES
)

set(IFACES
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
#qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
#qt4_wrap_ui(SOURCES ${IFACES})

#add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

#target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



