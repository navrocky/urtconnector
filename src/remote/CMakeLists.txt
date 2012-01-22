set(TARGET remote)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

set(HEADERS
    remote.h
    storage.h
    manager.h
    settings.h
    backends/binary_file.h
    backends/json_file.h
    backends/gdocs/gdocs.h 
    sync_settings_form.h
)

set(MOC_HEADERS
    storage.h
    manager.h
    backends/gdocs/gdocs.h
    backends/gdocs/gdocs_widget.h
    sync_settings_form.h
)

set(SOURCES
    remote.cpp
    manager.cpp
    settings.cpp
    backends/binary_file.cpp
    backends/json_file.cpp
    backends/gdocs/gdocs.cpp
    sync_settings_form.cpp
)

set(IFACES
    sync_settings_form.ui
    backends/gdocs/gdocs_widget.ui
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} qjson)



