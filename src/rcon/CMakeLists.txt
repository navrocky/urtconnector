cl_define_headers(
    rcon.h
    rcon_completer.h
    rcon_connection.h
    rcon_settings.h
    rcon_settings_form.h
)

cl_define_moc(
    rcon.h
    rcon_completer.h
    rcon_connection.h
    rcon_settings.h
    rcon_settings_form.h
)

cl_define_sources(
    rcon.cpp
    rcon_completer.cpp
    rcon_connection.cpp
    rcon_settings_form.cpp
)

cl_define_interfaces(
    rcon_settings_form.ui
    rcon.ui
)