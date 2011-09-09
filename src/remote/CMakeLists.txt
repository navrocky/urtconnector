set(TARGET remote)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

set(HEADERS
    remote.h
    storage.h
    backends/binary_file.h
    backends/json_file.h
)


set(SOURCES
    remote.cpp
    backends/binary_file.cpp
    backends/json_file.cpp
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} qjson)



