cl_define_headers(
    anticheat.h
    sshot_output.h
    sshot_file_output.h
    sshot_ftp_output.h
    settings.h
    settings_widget.h
    tools.h
)

cl_define_moc(
    anticheat.h
    sshot_output.h
    sshot_file_output.h
    sshot_ftp_output.h
    settings_widget.h
)

cl_define_sources(
    anticheat.cpp
    sshot_output.cpp
    sshot_file_output.cpp
    sshot_ftp_output.cpp
    settings.cpp
    settings_widget.cpp
    tools.cpp
)

cl_define_interfaces(
    settings_widget.ui
)
