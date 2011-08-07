set(TARGET updater)

find_package(Qt4 REQUIRED QtCore)
include(${QT_USE_FILE})

set(HEADERS
    update_task.h
    update_dispatcher.h
)

set(MOC_HEADERS
    update_task.h
    update_dispatcher.h
)

set(SOURCES
    update_task.cpp
    update_dispatcher.cpp
)

set(IFACES
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} 
    common
)
