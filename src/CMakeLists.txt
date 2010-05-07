set(TARGET urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})

find_package( Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore QtGui QtXml)
include(${QT_USE_FILE})

set(URT_DATADIR ${CMAKE_INSTALL_PREFIX}/share/urtconnector/)

configure_file(config.h.in config.h)

add_subdirectory(geoip)
add_subdirectory(qtsingleapplication)

set(HEADERS
    about_dialog.h
    application.h
    app_options.h
    debug_help.h
    exception.h
    launcher.h
    main_window.h
    options_dialog.h
    player_info.h
    push_button_action_link.h
    server_id.h
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
    server_tree_widget.h
)

set(MOC_HEADERS
    about_dialog.h
    application.h
    main_window.h
    options_dialog.h
    push_button_action_link.h
    server_list_widget.h
    server_options_dialog.h
    launcher.h
    item_view_dblclick_action_link.h
    qstat_updater.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
    clipper.h
    server_tree_widget.h
)

set(SOURCES
    main.cpp
    about_dialog.cpp
    application.cpp
    app_options.cpp
    debug_help.cpp
    exception.cpp
    launcher.cpp
    main_window.cpp
    options_dialog.cpp
    player_info.cpp
    push_button_action_link.cpp
    server_id.cpp
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
    server_tree_widget.cpp
)

set(IFACES
    about_dialog.ui
    main_window.ui
    options_dialog.ui
    server_list_widget.ui
    server_options_dialog.ui
    jobs/job_monitor.ui
    jobs/job_item.ui
)

set(RESOURCES
    images.qrc
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_executable(${TARGET} WIN32 ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES} geoip qtsingleapplication)

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES icons/urtconnector.png DESTINATION share/icons)
install(FILES icons/urtconnector.png DESTINATION share/pixmaps)
install(FILES urtconnector.desktop DESTINATION share/applications)
install(FILES GeoIP.dat DESTINATION share/urtconnector)

