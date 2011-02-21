set(TARGET tabs)

find_package(Qt4 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

set(HEADERS
    tab_context.h
    main_tab.h
    filtered_tab.h
    server_list_common_tab.h
    bookmark_tab.h
    server_list_tab.h
    status_item_delegate.h
)

set(MOC_HEADERS
    main_tab.h
    filtered_tab.h
    server_list_common_tab.h
    bookmark_tab.h
    server_list_tab.h
)

set(SOURCES
    tab_context.cpp
    main_tab.cpp
    filtered_tab.cpp
    server_list_common_tab.cpp
    bookmark_tab.cpp
    server_list_tab.cpp
    status_item_delegate.cpp
)

set(IFACES
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} 
    common
    filters
    settings
)
