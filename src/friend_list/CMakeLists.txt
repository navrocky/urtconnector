set(TARGET friend_list)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    friend_list_widget.h
    friend_record.h
    friend_list.h
    friend_prop_dialog.h
    friend_list_db_saver.h
)

set(MOC_HEADERS
    friend_list_widget.h
    friend_list.h
    friend_prop_dialog.h
    friend_list_db_saver.h
)

set(SOURCES
    friend_list_widget.cpp
    friend_record.cpp
    friend_list.cpp
    friend_prop_dialog.cpp
    friend_list_db_saver.cpp
)

set(IFACES
    friend_prop_dialog.ui
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET} ${QT_LIBRARIES}
    common
    tabs
)
