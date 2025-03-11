add_library(scheduler_lib
    process.h
    fcfs/fcfs_scheduler.cpp
    fcfs/fcfs_scheduler.h
)

add_executable(os_scheduler 
    main.cpp
)

target_link_libraries(os_scheduler
    PRIVATE scheduler_lib
) 