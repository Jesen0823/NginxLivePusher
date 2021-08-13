
cmake_minimum_required(VERSION 3.10.2)
# 因为rtmpdump是由c编写，使用C编译
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_library(
        rtmp #编译要生成的静态库名
        STATIC
        amf.c
        hashswf.c
        log.c
        parseurl.c
        rtmp.c
)