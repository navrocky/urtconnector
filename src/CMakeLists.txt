cl_define_target(urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
else()
    set(Boost_USE_STATIC_LIBS OFF)
endif()

if (UNIX)
    find_package(X11)
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

#cl_qt_required(QtCore QtGui QtXml QtNetwork)
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

cl_add_subdirectory(updater)
cl_add_subdirectory(rcon)
cl_add_subdirectory(jobs)
cl_add_subdirectory(geoip)
cl_add_subdirectory(qtsingleapplication)
cl_add_subdirectory(cl)
cl_add_subdirectory(filters)
cl_add_subdirectory(settings)
cl_add_subdirectory(preferences)
cl_add_subdirectory(common)
cl_add_subdirectory(history)
cl_add_subdirectory(anticheat)
cl_add_subdirectory(database)
cl_add_subdirectory(launcher)
cl_add_subdirectory(friend_list)
cl_add_subdirectory(tabs)
cl_add_subdirectory(setting_forms)
cl_add_subdirectory(tracking)
cl_add_subdirectory(remote)

cl_define_headers(
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
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    server_info_manager.h
)

cl_define_moc(
    about_dialog.h
    application.h
    main_window.h
    push_button_action_link.h
    server_options_dialog.h
    qstat_updater.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    server_info_manager.h
    gdoc.h
)

cl_define_sources(
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
    job_update_selected.cpp
    job_update_from_master.cpp
    clipper.cpp
    server_info_manager.cpp
)

cl_define_interfaces(
    about_dialog.ui
    main_window.ui
    server_options_dialog.ui
)

cl_define_resources(
    images.qrc
)

cl_define_deps(
    geoip
    qtsingleapplication
    clexcept
    clsyslog
    preferences
    common
    database
)

qt4_wrap_cpp(CL_SOURCES ${CL_MOC})
qt4_wrap_ui(CL_SOURCES ${CL_IFACES})
qt4_add_resources(CL_SOURCES ${CL_RESOURCES})

add_executable(${TARGET} WIN32 ${CL_HEADERS} ${CL_SOURCES})
target_link_libraries(${TARGET} ${QT_LIBRARIES} ${Boost_LIBRARIES} ${X11_LIBRARIES} ${CL_DEPS})

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES icons/urtconnector.png DESTINATION share/icons)
install(FILES icons/urtconnector.png DESTINATION share/pixmaps)
install(FILES icons/urtconnector-48.png DESTINATION share/icons/hicolor/48x48/apps RENAME urtconnector.png)
install(FILES sounds/notify1.ogg DESTINATION share/urtconnector/sounds)
install(FILES sounds/notify2.ogg DESTINATION share/urtconnector/sounds)
install(FILES sounds/notify3.ogg DESTINATION share/urtconnector/sounds)
install(FILES urtconnector.desktop DESTINATION share/applications)
install(FILES GeoIP.dat DESTINATION share/urtconnector)

