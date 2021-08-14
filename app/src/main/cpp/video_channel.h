//
// Created by X1 Carbon on 2021/8/14.
//

#ifndef NGINXLIVEPUSHER_VIDEO_CHANNEL_H
#define NGINXLIVEPUSHER_VIDEO_CHANNEL_H

#include "librtmp//rtmp.h"

class VideoChannelC {
    // 接口VideoCallback 传参类型RTMPPacket
    // 作用：传递packet,以便将packet加入队列
    typedef void (*VideoCallback)(RTMPPacket *packet);

public:
    void setVideoEncInfo(int width, int height, int fps, int bitrate);

    ~VideoChannelC();

    void encodeData(int8_t *data);

    void setVideoCallback(VideoCallback videoCallback);

private:
    // 保存变量
    int mWidth;
    int mHeight;
    int mFps;
    int mBitrate;
    int ySize;
    int uvSize;
    // 一帧图片，x264编码保存位置
    x264_picture_t *pic_in;
    VideoCallback videoCallback;

    void sendFrame(int type, uint8_t *payload, int i_payload);

    void sendSpsPps(uint8_t sps[100], uint8_t pps[100], int len, int pps_len);

    x264_t *videoCodec; // 初始化得到的x264解码器
};

#endif //NGINXLIVEPUSHER_VIDEO_CHANNEL_H
