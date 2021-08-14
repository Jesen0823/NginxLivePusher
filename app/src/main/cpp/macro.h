//
// Created by X1 Carbon on 2021/8/14.
// 打印日志

#ifndef NGINXLIVEPUSHER_MACRO_H
#define NGINXLIVEPUSHER_MACRO_H

#include <android/log.h>
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"LivePush",__VA_ARGS__)
//宏函数
#define DELETE(obj) if(obj){ delete obj; obj = 0; }

#endif //NGINXLIVEPUSHER_MACRO_H
