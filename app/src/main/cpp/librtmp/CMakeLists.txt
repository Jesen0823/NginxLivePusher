
cmake_minimum_required(VERSION 3.10.2)
# 因为rtmpdump是由c编写，使用C编译
#[[ 会因为找不到openssl编译不过，添加参数NO_CRYPTO
    使不走openssl加密流程。
    hashswf.c
    atal error: 'openssl/ssl.h' file not found
    对应
    rtmp.h  29行 define CRYPTO
]]
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_CRYPTO")

add_library(
        rtmp #编译要生成的静态库名
        STATIC
        amf.c
        hashswf.c
        log.c
        parseurl.c
        rtmp.c
)