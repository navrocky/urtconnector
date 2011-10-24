set(TARGET remote)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

set(HEADERS
    remote.h
    storage.h
    manager.h
    backends/binary_file.h
    backends/json_file.h
    backends/gdocs/gdocs.h 
)

set(MOC_HEADERS
    storage.h
    backends/gdocs/gdocs.h
)

set(SOURCES
    remote.cpp
    manager.cpp
    backends/binary_file.cpp
    backends/json_file.cpp
    backends/gdocs/gdocs.cpp
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} qjson)



