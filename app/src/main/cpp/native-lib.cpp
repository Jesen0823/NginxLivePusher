#include <jni.h>
#include <string>

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
    // TODO: implement native_init()
}