cl_define_headers(
    pointers.h
    filter.h
    filter_factory.h
    filter_list.h
    composite_filter.h
    filter_edit_widget.h
    reg_filters.h
    tools.h
    game_type_filter.h
    hide_empty_filter.h
    hide_full_filter.h
    regexp_filter.h
    online_clans_filter.h
    hide_private_filter.h
    weapon_filter.h
    ping_filter.h
    custom_filter.h
    player_filter.h
    friend_filter.h
    minimum_empty_slots_filter.h
    game_version_filter.h
)

cl_define_moc(
    filter.h
    filter_list.h
    composite_filter.h
    filter_edit_widget.h
    game_type_filter.h
    hide_empty_filter.h
    hide_full_filter.h
    regexp_filter.h
    online_clans_filter.h
    hide_private_filter.h
    weapon_filter.h
    ping_filter.h
    custom_filter.h
    player_filter.h
    friend_filter.h
    minimum_empty_slots_filter.h
    game_version_filter.h
)

cl_define_sources(
    filter.cpp
    filter_factory.cpp
    filter_list.cpp
    composite_filter.cpp
    filter_edit_widget.cpp
    reg_filters.cpp
    tools.cpp
    game_type_filter.cpp
    hide_empty_filter.cpp
    hide_full_filter.cpp
    regexp_filter.cpp
    online_clans_filter.cpp
    hide_private_filter.cpp
    weapon_filter.cpp
    ping_filter.cpp
    custom_filter.cpp
    player_filter.cpp
    friend_filter.cpp
    minimum_empty_slots_filter.cpp
    game_version_filter.cpp
)
