cl_define_headers(
    reg_actions.h
    show_query_action.h
    show_message_action.h
    connect_action.h
    select_server_action.h
    play_sound_action.h
)

cl_define_moc(
    show_query_action.h
    show_message_action.h
    connect_action.h
    select_server_action.h
    play_sound_action.h
)

cl_define_sources(
    reg_actions.cpp
    show_query_action.cpp
    show_message_action.cpp
    connect_action.cpp
    select_server_action.cpp
    play_sound_action.cpp
)
