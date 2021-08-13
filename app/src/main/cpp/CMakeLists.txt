
cmake_minimum_required(VERSION 3.10.2)

project("nginxlivepusher")

add_library( 
        nginxlivepusher
        SHARED
        native-lib.cpp)

find_library( 
        log-lib
        log)

target_link_libraries( 
        nginxlivepusher
        ${log-lib})