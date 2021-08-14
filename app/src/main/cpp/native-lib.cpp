#include <jni.h>
#include <string>
#include "x264.h"
#include "librtmp//rtmp.h"
#include "video_channel.h"

VideoChannelC *videoChannel;

extern "C" JNIEXPORT jstring JNICALL
Java_com_jesen_nginxlivepusher_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1init(JNIEnv *env, jobject thiz) {
    videoChannel = new VideoChannelC;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1start__(JNIEnv *env, jobject thiz) {
    // TODO: implement native_start()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject thiz,
                                                                     jint width, jint height,
                                                                     jint fps, jint bitrate) {
    if (!videoChannel){
        return;
    }
    videoChannel->setVideoEncInfo(width,height,fps,bitrate);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1start__Ljava_lang_String_2(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jstring path) {
    // TODO: implement native_start()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1pushVideo(JNIEnv *env, jobject thiz,
                                                               jbyteArray data) {
    // TODO: implement native_pushVideo()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1pushAudio(JNIEnv *env, jobject thiz,
                                                               jbyteArray bytes) {
    // TODO: implement native_pushAudio()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1setAudioEncInfo(JNIEnv *env, jobject thiz,
                                                                     jint i, jint channels) {
    // TODO: implement native_setAudioEncInfo()
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_getInputSamples(JNIEnv *env, jobject thiz) {
    // TODO: implement getInputSamples()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1release(JNIEnv *env, jobject thiz) {
    // TODO: implement native_release()
}