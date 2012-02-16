set(TARGET urtc_qstat)

set(HEADERS
    qstat/a2s.h
    qstat/bfbc2.h
    qstat/config.h
    qstat/cube2.h
    qstat/debug.h
    qstat/doom3.h
    qstat/fl.h
    qstat/gps.h
    qstat/gs2.h
    qstat/gs3.h
    qstat/haze.h
    qstat/md5.h
    qstat/mumble.h
    qstat/packet_manip.h
    qstat/qserver.h
    qstat/terraria.h
    qstat/tm.h
    qstat/ts2.h
    qstat/qstat.h
    qstat/tee.h
    qstat/ut2004.h
    qstat/ventrilo.h
    qstat/wic.h
)

set(SOURCES
    qstat/a2s.c
    qstat/bfbc2.c
    qstat/config.c
    qstat/cube2.c
    qstat/debug.c
    qstat/doom3.c
    qstat/fl.c
    qstat/gps.c
    qstat/gs2.c
    qstat/gs3.c
    qstat/haze.c
    qstat/hcache.c
    qstat/md5.c
    qstat/mumble.c
    qstat/ottd.c
    qstat/ottd.h
    qstat/packet_manip.c
    qstat/qserver.c
    qstat/qstat.c
    qstat/tee.c
    qstat/template.c
    qstat/terraria.c
    qstat/tm.c
    qstat/ts2.c
    qstat/ts3.c
    qstat/ts3.h
    qstat/ut2004.c
    qstat/ventrilo.c
    qstat/wic.c
)

add_executable(${TARGET} ${HEADERS} ${SOURCES})

#target_link_libraries( ${TARGET} ${QT_LIBRARIES} )

install(TARGETS ${TARGET} RUNTIME DESTINATION bin)

