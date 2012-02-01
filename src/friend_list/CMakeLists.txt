cl_define_headers(
    friend_list_widget.h
    friend_record.h
    friend_list.h
    friend_prop_dialog.h
    friend_list_db_saver.h
)

cl_define_moc(
    friend_list_widget.h
    friend_list.h
    friend_prop_dialog.h
    friend_list_db_saver.h
)

cl_define_sources(
    friend_list_widget.cpp
    friend_record.cpp
    friend_list.cpp
    friend_prop_dialog.cpp
    friend_list_db_saver.cpp
)

cl_define_interfaces(
    friend_prop_dialog.ui
)
