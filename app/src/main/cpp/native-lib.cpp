#include <jni.h>
#include <string>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "video_channel.h"
#include "macro.h"
#include "safe_queue.h"
#include "audio_channel.h"

VideoChannelC *videoChannel;
AudioChannelC *audioChannel;
int isStart = 0;

pthread_t pid;
int32_t start_time;
int readyPushing = 0; // 开始推流的标记位

SafeQueue<RTMPPacket *> packetQueue;

void releasePacket(RTMPPacket *packet){
    if (packet){
        RTMPPacket_Free(packet);
        delete packet;
        packet = 0;
    }
}

void callback(RTMPPacket *packet){
    if (packet){
        // 设置时间戳
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        // 加入队列
        packetQueue.put(packet);
        LOGE("callback the Queue size:",packetQueue.size());
    }
}

// 线程中要执行的方法
void *start(void *args){
    char *url = static_cast<char *>(args);
    // 链接服务器
    RTMP *rtmp = 0;
    rtmp = RTMP_Alloc();
    if (!rtmp){
        LOGE("alloc rtmp failed.");
        return NULL;
    }
    RTMP_Init(rtmp);
    int ret = RTMP_SetupURL(rtmp, url);
    if (!ret){
        LOGE("rtmp set url failed.");
        return NULL;
    }
    rtmp->Link.timeout = 5;
    RTMP_EnableWrite(rtmp);
    ret = RTMP_Connect(rtmp,0); // 通过socket方式链接服务器
    if (!ret){
        LOGE("rtmp connect server failed,url: ",url);
        return NULL;
    }
    // 链接流
    ret = RTMP_ConnectStream(rtmp,0);
    if (!ret){
        LOGE("rtmp connect stream failed,url: ",url);
        return NULL;
    }

    // 直播开始的时间
    start_time = RTMP_GetTime();
    readyPushing = 1; //可以开始推流了
    packetQueue.setWork(1);
    RTMPPacket *packet = 0;
    callback(audioChannel->getAudioTag());
    while (readyPushing){
        // 队列中取数据(packet)
        packetQueue.get(packet);
        LOGE("get packet from queue.");
        if (!readyPushing){
            break;
        }
        if (!packet){
            continue;
        }
        packet->m_nInfoField2 = rtmp->m_stream_id;
        // 发送数据包
        int ret = RTMP_SendPacket(rtmp, packet,1);
        LOGE("rtmp send packet result:",ret);
        // 释放packet
        releasePacket(packet);
    }
    isStart = 0;
    readyPushing = 0;
    packetQueue.setWork(0);
    packetQueue.clear();
    if (rtmp){
        RTMP_Close(rtmp); // 关闭链接
        RTMP_Free(rtmp);
    }
    delete (url);
    return 0;
}

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
    videoChannel->setVideoCallback(callback);
    audioChannel = new AudioChannelC();
    audioChannel->setAudioCallback(callback);
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
                                                                                jstring path_) {
    const char* path = env->GetStringUTFChars(path_,0);
    if (isStart){
        return;
    }
    // 用来保存path
    char *url = new char [strlen(path)+1];
    stpcpy(url, path);

    // 开启线程 执行start方法 传参url
    pthread_create(&pid, 0, start, url);

    env->ReleaseStringUTFChars(path_, path);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1pushVideo(JNIEnv *env, jobject thiz,
                                                               jbyteArray data_) {
    if (!videoChannel || !readyPushing){
        return;
    }
    jbyte *data = env->GetByteArrayElements(data_,NULL);

    videoChannel->encodeData(data);

    env->ReleaseByteArrayElements(data_,data,0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1pushAudio(JNIEnv *env, jobject thiz,
                                                               jbyteArray bytes_) {
   jbyte *data = env->GetByteArrayElements(bytes_,NULL);

   if (!audioChannel || !readyPushing){
       return;
   }
    audioChannel->encodeData(data);

   env->ReleaseByteArrayElements(bytes_,data,0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1setAudioEncInfo(JNIEnv *env, jobject thiz,
                                                                     jint sampleRateInHz, jint channels) {
    if (audioChannel){
        audioChannel->setAudioEncInfo(sampleRateInHz, channels);
    }
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_getInputSamples(JNIEnv *env, jobject thiz) {
    if (audioChannel) {
        return audioChannel->getInputSamples();
    }
    return -1;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jesen_nginxlivepusher_av_LivePusher_native_1release(JNIEnv *env, jobject thiz) {
    DELETE(videoChannel);
    DELETE(audioChannel);
}