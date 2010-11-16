set(TARGET urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package( Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4)
set(QT_USE_QTXML true)
set(QT_USE_QTNETWORK true)

include(${QT_USE_FILE})

if (WIN32)
    set(URT_DATADIR "")
else()
    set(URT_DATADIR /usr/share/urtconnector/)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_definitions(-DUSE_SINGLE_APP)

configure_file(config.h.in config.h)

add_subdirectory(geoip)
add_subdirectory(qtsingleapplication)
add_subdirectory(cl)
add_subdirectory(filters)
add_subdirectory(settings)
add_subdirectory(common)
add_subdirectory(history)
add_subdirectory(anticheat)

set(HEADERS
    about_dialog.h
    application.h
    app_options.h
    debug_help.h
    exception.h
    launcher/launcher.h
    main_window.h
    options_dialog.h
    player_info.h
    push_button_action_link.h
    server_info.h
    server_list.h
    server_list_saver.h
    server_options.h
    server_list_widget.h
    server_options_dialog.h
    str_convert.h
    app_options_saver.h
    server_info_html.h
    config.h
    item_view_dblclick_action_link.h
    qstat_updater.h
    qstat_options.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    tools.h
    launcher/tools.h
)

set(MOC_HEADERS
    about_dialog.h
    application.h
    main_window.h
    options_dialog.h
    push_button_action_link.h
    server_list_widget.h
    server_options_dialog.h
    launcher/launcher.h
    item_view_dblclick_action_link.h
    qstat_updater.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    rcon/rcon.h
    rcon/rcon_settings_form.h
    rcon/rcon_completer.h
)

set(SOURCES
    main.cpp
    about_dialog.cpp
    application.cpp
    app_options.cpp
    debug_help.cpp
    exception.cpp
    launcher/launcher.cpp
    main_window.cpp
    options_dialog.cpp
    player_info.cpp
    push_button_action_link.cpp
    server_info.cpp
    server_list.cpp
    server_list_saver.cpp
    server_options.cpp
    server_list_widget.cpp
    server_options_dialog.cpp
    app_options_saver.cpp
    server_info_html.cpp
    item_view_dblclick_action_link.cpp
    qstat_updater.cpp
    qstat_options.cpp
    jobs/job.cpp
    jobs/job_queue.cpp
    jobs/job_monitor.cpp
    job_update_selected.cpp
    job_update_from_master.cpp
    clipper.cpp
    tools.cpp
    launcher/tools.cpp
    rcon/rcon.cpp
    rcon/rcon_settings_form.cpp
    rcon/rcon_completer.cpp
)

set(IFACES
    about_dialog.ui
    main_window.ui
    options_dialog.ui
    server_options_dialog.ui
    jobs/job_monitor.ui
    jobs/job_item.ui
    rcon/rcon.ui
    rcon/rcon_settings_form.ui
)

set(RESOURCES
    images.qrc
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES} 
    geoip
    qtsingleapplication
    clsyslog
    filters
    settings
    common
    history
    anticheat
)

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES icons/urtconnector.png DESTINATION share/icons)
install(FILES icons/urtconnector.png DESTINATION share/pixmaps)
install(FILES icons/urtconnector-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME urtconnector.png)
install(FILES urtconnector.desktop DESTINATION share/applications)
install(FILES GeoIP.dat DESTINATION share/urtconnector)



