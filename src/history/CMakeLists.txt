cl_define_headers(
    history.h
    history_item.h
    history_widget.h
    history_settings_form.h
)

cl_define_moc(
    history_widget.h
    history.h
    history_settings_form.h
)

cl_define_sources(
    history.cpp
    history_item.cpp
    history_widget.cpp
    history_settings_form.cpp
)

cl_define_interfaces(
    history_settings_form.ui
)
