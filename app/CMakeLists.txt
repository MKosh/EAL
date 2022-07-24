add_executable(EALapplication EALapplication.cc)
target_include_directories(EALapplication PUBLIC "../include/EAL/")
target_link_libraries(EALapplication LINK_PUBLIC EAL)
