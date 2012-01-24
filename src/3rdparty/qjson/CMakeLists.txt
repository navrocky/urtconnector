set(TARGET qjson)

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})

add_definitions(-DQJSON_MAKEDLL)

set(HEADERS
    json_parser.hh
    json_scanner.h
    parser.h
    parserrunnable.h
    qjson_debug.h
    qjson_export.h
    qobjecthelper.h
    serializer.h
    serializerrunnable.h
    stack.hh
)


set(MOC_HEADERS
    parserrunnable.h
    serializerrunnable.h
)

set(SOURCES
    json_parser.cc
    json_scanner.cpp
    parser.cpp
    parserrunnable.cpp
    qobjecthelper.cpp
    serializer.cpp
    serializerrunnable.cpp
)

qt4_wrap_cpp(SOURCES ${MOC_HEADERS})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )



