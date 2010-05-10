set(TARGET settings)

project(${TARGET})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})

find_package( Boost COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 REQUIRED QtCore )
include(${QT_USE_FILE})


set(HEADERS
    settings.h
    detail.h
)

set(MOC_HEADERS
)

set(SOURCES
    settings.cpp
    detail.cpp
)

set(IFACES
)

set(RESOURCES
)

#qt4_add_resources(SOURCES ${RESOURCES})
#qt4_wrap_cpp(SOURCES ${MOC_HEADERS})
#qt4_wrap_ui(SOURCES ${IFACES})

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_link_libraries( ${TARGET} ${QT_LIBRARIES} )

#install(TARGETS ${TARGET} RUNTIME DESTINATION bin)




