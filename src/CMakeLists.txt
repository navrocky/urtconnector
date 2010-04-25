set(TARGET urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})

find_package( Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore QtGui QtXml)
include(${QT_USE_FILE})

configure_file(config.h.in config.h)

set(HEADERS
    aboutdialog.h
    application.h
    app_options.h
    debughelp.h
    exception.h
    launcher.h
    main_window.h
    optionsdialog.h
    playerinfo.h
    pushbuttonactionlink.h
    server_id.h
    server_info.h
    serverlistcustom.h
    serverlistqstat.h
    serveroptions.h
    servlistwidget.h
    servoptsdialog.h
    xmlreaddata.h
    strconvert.h
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
)

set(MOC_HEADERS
    aboutdialog.h
    application.h
    main_window.h
    optionsdialog.h
    pushbuttonactionlink.h
    serverlistcustom.h
    serverlistqstat.h
    servlistwidget.h
    servoptsdialog.h
    launcher.h
    item_view_dblclick_action_link.h
    qstat_updater.h
    jobs/job.h
    jobs/job_queue.h
    jobs/job_monitor.h
    job_update_selected.h
    job_update_from_master.h
)

set(SOURCES
    main.cpp
    aboutdialog.cpp
    application.cpp
    app_options.cpp
    debughelp.cpp
    exception.cpp
    launcher.cpp
    main_window.cpp
    optionsdialog.cpp
    playerinfo.cpp
    pushbuttonactionlink.cpp
    server_id.cpp
    server_info.cpp
    serverlistcustom.cpp
    serverlistqstat.cpp
    serveroptions.cpp
    servlistwidget.cpp
    servoptsdialog.cpp
    xmlreaddata.cpp
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
)

set(IFACES
    aboutdialog.ui
#    mainwindow_concept.ui
    main_window.ui
    optionsdialog.ui
    servlistwidget.ui
    servoptsdialog.ui
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

target_link_libraries(${TARGET} ${QT_LIBRARIES})

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(FILES icons/urtconnector.png DESTINATION share/icons)
install(FILES icons/urtconnector.png DESTINATION share/pixmaps)
install(FILES urtconnector.desktop DESTINATION share/applications)
