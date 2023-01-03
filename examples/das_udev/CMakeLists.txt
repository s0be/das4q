cmake_minimum_required(VERSION 3.15.0)

add_executable(das_udev ./das_udev.c)
target_link_libraries(das_udev das4q cjson)