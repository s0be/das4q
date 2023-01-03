cmake_minimum_required(VERSION 3.15.0)

add_library(das4q ./src/libdas4q.c)
target_include_directories(das4q PUBLIC include/)
target_link_libraries(das4q usb-1.0)