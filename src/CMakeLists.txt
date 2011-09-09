set(TARGET urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
else()
    set(Boost_USE_STATIC_LIBS OFF)
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.39.0 REQUIRED COMPONENTS
    program_options
    thread
    date_time
    filesystem
    system
)
include_directories(${Boost_INCLUDE_DIRS})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

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

add_subdirectory(3rdparty)
add_subdirectory(geoip)
add_subdirectory(qtsingleapplication)
add_subdirectory(cl)
add_subdirectory(filters)
add_subdirectory(settings)
add_subdirectory(preferences)
add_subdirectory(common)
add_subdirectory(history)
add_subdirectory(anticheat)
add_subdirectory(database)
add_subdirectory(launcher)
add_subdirectory(friend_list)
add_subdirectory(tabs)
add_subdirectory(setting_forms)
add_subdirectory(updater)
add_subdirectory(tracking)
add_subdirectory(remote)

set(HEADERS
    about_dialog.h
    application.h
    app_options.h
    debug_help.h
    main_window.h
    push_button_action_link.h
    server_list_saver.h
    server_options_dialog.h
    app_options_saver.h
    server_info_html.h
    config.h
    qstat_updater.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    launcher/tools.h
    server_info_manager.h
)

set(MOC_HEADERS
    about_dialog.h
    application.h
    main_window.h
    push_button_action_link.h
    server_options_dialog.h
    qstat_updater.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    rcon/rcon_connection.h
    rcon/rcon.h
    rcon/rcon_settings_form.h
    rcon/rcon_completer.h
    server_info_manager.h
)

set(SOURCES
    main.cpp
    about_dialog.cpp
    application.cpp
    app_options.cpp
    debug_help.cpp
    main_window.cpp
    push_button_action_link.cpp
    server_list_saver.cpp
    server_options_dialog.cpp
    app_options_saver.cpp
    server_info_html.cpp
    qstat_updater.cpp
    jobs/job.cpp
    jobs/job_queue.cpp
    jobs/job_monitor.cpp
    job_update_selected.cpp
    job_update_from_master.cpp
    clipper.cpp
    rcon/rcon_connection.cpp
    rcon/rcon.cpp
    rcon/rcon_settings_form.cpp
    rcon/rcon_completer.cpp
    server_info_manager.cpp
)

set(IFACES
    about_dialog.ui
    main_window.ui
    server_options_dialog.ui
    jobs/job_monitor.ui
    jobs/job_item.ui
    rcon/rcon.ui
    rcon/rcon_settings_form.ui
    setting_forms/launch_settings_form.ui
    setting_forms/application_settings_form.ui
)

set(RESOURCES
    images.qrc
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES} ${Boost_LIBRARIES}
    geoip
    qtsingleapplication
    clexcept
    clsyslog
    filters
    settings
    preferences
    common
    history
    anticheat
    database
    launcher
    tabs
    friend_list
    setting_forms
    updater
    tracking
    phonon
    remote
)

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES icons/urtconnector.png DESTINATION share/icons)
install(FILES icons/urtconnector.png DESTINATION share/pixmaps)
install(FILES icons/urtconnector-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME urtconnector.png)
install(FILES urtconnector.desktop DESTINATION share/applications)
install(FILES GeoIP.dat DESTINATION share/urtconnector)

