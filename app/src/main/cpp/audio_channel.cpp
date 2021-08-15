//
// Created by X1 Carbon on 2021/8/13.
//
#include <pty.h>
#include <jni.h>
#include <cstring>
#include "audio_channel.h"
#include "librtmp/rtmp.h"
#include "faac.h"
#include "macro.h"

RTMPPacket *AudioChannelC::getAudioTag() {
    u_char *buf;
    u_long len;
//     编码器信息     解码
    faacEncGetDecoderSpecificInfo(audioCodec, &buf, &len);
    int bodySizw = 2 + len;
    int bodySize = 2 + len;
    RTMPPacket *packet = new RTMPPacket;
    RTMPPacket_Alloc(packet, bodySize);
    //双声道
    packet->m_body[0] = 0xAF;
    if (mChannels == 1) {
        packet->m_body[0] = 0xAE;
    }
    packet->m_body[1] = 0x00;
    //图片数据
    memcpy(&packet->m_body[2], buf, len);

    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = 0x11;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    return packet;
}

void AudioChannelC::encodeData(int8_t *data) {
    int byteLen =  faacEncEncode(audioCodec, reinterpret_cast<int32_t *>(data), inputSamples, buffer, maxOutputBytes);
    if (byteLen > 0){
        RTMPPacket *packet = new RTMPPacket;
        int bodySize = 2 + byteLen;
        RTMPPacket_Alloc(packet, bodySize);
        packet->m_body[0] = 0xAF;
        if (mChannels == 1){
            packet->m_body[0] = 0xAE;
        }
        // 编码出的声音都是0x01
        packet->m_body[1] = 0x01;
        memcpy(&packet->m_body[2], buffer, byteLen);

        // aac
        packet->m_hasAbsTimestamp = 0;
        packet->m_nBodySize = bodySize;
        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_nChannel = 0x11;
        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        audioCallback(packet);
    }
}

// 初始化faac编码器
void AudioChannelC::setAudioEncInfo(int samplesInHZ, int channels) {
   // 初始化音频解码器
    audioCodec =  faacEncOpen(samplesInHZ, channels, &inputSamples,&maxOutputBytes);
    // 设置编码器参数
    faacEncConfigurationPtr config =faacEncGetCurrentConfiguration(audioCodec);
    config->mpegVersion = MPEG4;
    config->aacObjectType = LOW;
    config->inputFormat = FAAC_INPUT_16BIT;
    // 编码原始数据
    config->outputFormat = 0;
    faacEncSetConfiguration(audioCodec, config);
    buffer = new u_char [maxOutputBytes];
}

void AudioChannelC::setAudioCallback(AudioCallback audioCallback) {
    this->audioCallback = audioCallback;
}

int AudioChannelC::getInputSamples(){
    return inputSamples;
}

AudioChannelC::~AudioChannelC() {
    DELETE(buffer);
    // 释放编码器
    if (audioCodec){
        faacEncClose(audioCodec);
        audioCodec = 0;
    }
}
