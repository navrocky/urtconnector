set(TARGET common)

find_package(Qt4 COMPONENTS Phonon QtCore)
include(${QT_USE_FILE})

if (NOT QT_PHONON_FOUND)
    find_library(PHONON phonon)
    if (PHONON)
        list(APPEND QT_LIBRARIES ${PHONON})
    else()
        message(FATAL_ERROR "Phonon not found")
    endif()
endif()

set(HEADERS
    scoped_tools.h
    server_id.h
    state_settings.h
    qt_syslog.h
    string_escape.h
    exception.h
    server_info.h
    server_list.h
    player_info.h
    implicit_sharing.h
    qaccumulatingconnection.h
    str_convert.h
    qbuttonlineedit.h
    main_tab_widget.h
    server_bookmark.h
    item_view_dblclick_action_link.h
    tools.h
    list_caption_updater.h
    qtreesearcher.h
    class_list.h
    notificator.h
    play_sound.h
    qstat_options.h
    json_tools.h
)

set(MOC_HEADERS
    server_list.h
    qaccumulatingconnection.h
    qbuttonlineedit.h
    main_tab_widget.h
    server_bookmark.h
    item_view_dblclick_action_link.h
    tools.h
    qtreesearcher.h
    notificator.h
)

set(SOURCES
    server_id.cpp
    state_settings.cpp
    qt_syslog.cpp
    string_escape.cpp
    exception.cpp
    server_info.cpp
    server_list.cpp
    qaccumulatingconnection.cpp
    qbuttonlineedit.cpp
    main_tab_widget.cpp
    server_bookmark.cpp
    item_view_dblclick_action_link.cpp
    tools.cpp
    list_caption_updater.cpp
    qtreesearcher.cpp
    notificator.cpp
    play_sound.cpp
    qstat_options.cpp
    json_tools.cpp
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

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES} qjson)
