set(TARGET history)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    history.h
    history_item.h
    history_widget.h
)

set(MOC_HEADERS
    history_widget.h
)

set(SOURCES
    history.cpp
    history_item.cpp
    history_widget.cpp
)

set(IFACES
    history_widget.ui
)

set(RESOURCES
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES})
