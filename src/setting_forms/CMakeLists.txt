set(TARGET setting_forms)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    launch_settings_form.h
    application_settings_form.h
    update_settings_form.h
)

set(MOC_HEADERS
    launch_settings_form.h
    application_settings_form.h
    update_settings_form.h
)

set(SOURCES
    launch_settings_form.cpp
    application_settings_form.cpp
    update_settings_form.cpp
)

set(IFACES
    launch_settings_form.ui
    application_settings_form.ui
    update_settings_form.ui
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES})
