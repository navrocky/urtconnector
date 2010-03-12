set(TARGET urtconnector)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED QtCore QtGui QtXml)
include(${QT_USE_FILE})

set(HEADERS
    aboutdialog.h
    application.h
    appoptions.h
    debughelp.h
    exception.h
    launcher.h
    mainwindow.h
    optionsdialog.h
    playerinfo.h
    pushbuttonactionlink.h
    qstatoptions.h
    serverid.h
    serverinfo.h
    serverlistcustom.h
    serverlistqstat.h
    serveroptions.h
    servlistwidget.h
    servoptsdialog.h
    xmlreaddata.h
    strconvert.h
    app_options_saver.h
)

set(MOC_HEADERS
    aboutdialog.h
    application.h
    mainwindow.h
    optionsdialog.h
    pushbuttonactionlink.h
    serverlistcustom.h
    serverlistqstat.h
    servlistwidget.h
    servoptsdialog.h
    launcher.h
)

set(SOURCES
    main.cpp
    aboutdialog.cpp
    application.cpp
    appoptions.cpp
    debughelp.cpp
    exception.cpp
    launcher.cpp
    mainwindow.cpp
    optionsdialog.cpp
    playerinfo.cpp
    pushbuttonactionlink.cpp
    qstatoptions.cpp
    serverid.cpp
    serverinfo.cpp
    serverlistcustom.cpp
    serverlistqstat.cpp
    serveroptions.cpp
    servlistwidget.cpp
    servoptsdialog.cpp
    xmlreaddata.cpp
    app_options_saver.cpp
)

set(IFACES
    aboutdialog.ui
#    mainwindow_concept.ui
    mainwindow.ui
    optionsdialog.ui
    servlistwidget.ui
    servoptsdialog.ui
)

set(RESOURCES
    images.qrc
)


qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_executable(${TARGET} ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES})
