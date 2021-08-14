
cmake_minimum_required(VERSION 3.10.2)

project("nginxlivepusher")

# 引入子CMakeLists.txt
add_subdirectory(${CMAKE_SOURCE_DIR}/librtmp)

add_library( 
        nginxlivepusher
        SHARED
        native-lib.cpp
        video_channel.cpp
        audio_channel.cpp)

include_directories(${CMAKE_SOURCE_DIR}/../../../include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}")

find_library( 
        log-lib
        log)

target_link_libraries( 
        nginxlivepusher
        rtmp # 引入子库
        ${log-lib}
        x264
        faac)