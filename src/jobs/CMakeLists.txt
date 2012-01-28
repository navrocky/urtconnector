cl_define_headers(
    job.h
    job_monitor.h
    job_queue.h
    pointers.h
)

cl_define_moc(
    job.h
    job_monitor.h
    job_queue.h
)

cl_define_sources(
    job.cpp
    job_monitor.cpp
    job_queue.cpp
)

cl_define_interfaces(
    job_item.ui
    job_monitor.ui
)