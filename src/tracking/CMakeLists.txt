set(TARGET tracking)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    context.h
    condition.h
    action.h
    task.h
    manager.h
    tasks_panel.h
    task_prop_dlg.h
    db_saver.h
    tools.h

    conditions/reg_conditions.h
    conditions/timer_condition.h
    conditions/server_filter_condition.h

    actions/reg_actions.h
    actions/show_query_action.h
    actions/show_message_action.h
    actions/connect_action.h
    actions/select_server_action.h
    actions/play_sound_action.h
)

set(MOC_HEADERS
    condition.h
    action.h
    task.h
    manager.h
    tasks_panel.h
    task_prop_dlg.h
    db_saver.h

    conditions/timer_condition.h
    conditions/server_filter_condition.h

    actions/show_query_action.h
    actions/show_message_action.h
    actions/connect_action.h
    actions/select_server_action.h
    actions/play_sound_action.h
)

set(SOURCES
    context.cpp
    condition.cpp
    action.cpp
    task.cpp
    manager.cpp
    tasks_panel.cpp
    task_prop_dlg.cpp
    db_saver.cpp
    tools.cpp

    conditions/reg_conditions.cpp
    conditions/timer_condition.cpp
    conditions/server_filter_condition.cpp

    actions/reg_actions.cpp
    actions/show_query_action.cpp
    actions/show_message_action.cpp
    actions/connect_action.cpp
    actions/select_server_action.cpp
    actions/play_sound_action.cpp
)

set(IFACES
    tasks_panel.ui
    task_prop_dlg.ui
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} 
    common
    updater
    tabs
)
