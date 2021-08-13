
cmake_minimum_required(VERSION 3.10.2)

project("nginxlivepusher")

# 引入子CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/librtmp)

add_library( 
        nginxlivepusher
        SHARED
        native-lib.cpp)

find_library( 
        log-lib
        log)

target_link_libraries( 
        nginxlivepusher
        rtmp # 引入子库
        ${log-lib})