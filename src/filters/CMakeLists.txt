set(TARGET filters)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

set(HEADERS
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
)

set(MOC_HEADERS
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
)

set(SOURCES
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
)

set(IFACES
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} clexcept clsyslog)



