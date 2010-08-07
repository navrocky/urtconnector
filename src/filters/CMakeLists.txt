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
)

set(MOC_HEADERS
    filter.h
    composite_filter.h
    filter_edit_widget.h
)

set(SOURCES
    filter.cpp
    filter_factory.cpp
    filter_list.cpp
    composite_filter.cpp
    filter_edit_widget.cpp
    reg_filters.cpp
)

set(IFACES
)

set(RESOURCES
)

qt4_add_resources(SOURCES ${RESOURCES})
qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



