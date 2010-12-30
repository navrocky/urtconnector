set(TARGET database)

find_package(SQLite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIR})

find_package(Qt4 REQUIRED QtCore)
include(${QT_USE_FILE})

set(HEADERS
    simple_database.h
    sqlite_database.h
)

set(SOURCES
    sqlite_database.cpp
    simple_database.cpp
)

set(IFACES
)

set(RESOURCES
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} ${SQLITE3_LIBRARIES})
