cl_add_subdirectory(actions)
cl_add_subdirectory(conditions)

cl_define_headers(
    context.h
    condition.h
    action.h
    task.h
    manager.h
    tasks_panel.h
    task_prop_dlg.h
    db_saver.h
    tools.h
)

cl_define_moc(
    condition.h
    action.h
    task.h
    manager.h
    tasks_panel.h
    task_prop_dlg.h
    db_saver.h
)

cl_define_sources(
    context.cpp
    condition.cpp
    action.cpp
    task.cpp
    manager.cpp
    tasks_panel.cpp
    task_prop_dlg.cpp
    db_saver.cpp
    tools.cpp
)

cl_define_interfaces(
    tasks_panel.ui
    task_prop_dlg.ui
)
