set(TARGET common)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

set(HEADERS
    scoped_tools.h
    server_id.h
    settings_base_widget.h
)

set(MOC_HEADERS
    settings_base_widget.h
)

set(SOURCES
    server_id.cpp
    settings_base_widget.cpp
)

if (${QT_VERSION_MAJOR}.${QT_VERSION_MINOR} VERSION_GREATER 4.5)

    set(MOC_HEADERS ${MOC_HEADERS}
        iconned_dock_style.h
    )

    set(SOURCES ${SOURCES}
        iconned_dock_style.cpp
    )
endif()

set(IFACES
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



