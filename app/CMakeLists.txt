add_executable(EALapplication EALapplication.cc)
target_include_directories(EALapplication PUBLIC "EAL/")
target_link_libraries(EALapplication LINK_PUBLIC EAL)
