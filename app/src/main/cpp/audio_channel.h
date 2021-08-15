//
// Created by X1 Carbon on 2021/8/15.
//

#ifndef NGINXLIVEPUSHER_AUDIO_CHANNEL_H
#define NGINXLIVEPUSHER_AUDIO_CHANNEL_H

#include <faac.h>
#include <jni.h>
#include <pty.h>
#include "librtmp/rtmp.h"

class AudioChannelC {
    typedef void (*AudioCallback)(RTMPPacket *packet);
public:

    void encodeData(int8_t *data);
    void setAudioEncInfo(int samplesInHZ, int channels);

    jint getInputSamples();
    RTMPPacket* getAudioTag();
    ~AudioChannelC();

    void setAudioCallback(AudioCallback audioCallback);
private:
    AudioCallback audioCallback;
    int mChannels;
    faacEncHandle audioCodec;
    u_long inputSamples;
    u_long maxOutputBytes;
    u_char *buffer = 0;
};

#endif //NGINXLIVEPUSHER_AUDIO_CHANNEL_H
