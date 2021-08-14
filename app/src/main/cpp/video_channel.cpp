//
// Created by X1 Carbon on 2021/8/13.
//
#include <x264.h>
#include <pty.h>
#include <cstring>
#include "video_channel.h"

void VideoChannelC::setVideoEncInfo(int width, int height, int fps, int bitrate) {
    mWidth = width;
    mHeight = height;
    mFps = fps;
    mBitrate = bitrate;
    ySize = width * height;
    uvSize = ySize / 4;

    // 初始化x264编码器
    x264_param_t param;
    x264_param_default_preset(&param, "ultrafast", "zerolatency");
    param.i_level_idc = 32; // 编码复杂度
    param.i_csp = X264_CSP_I420; // 输入数据格式,要从NV21转换成I420
    param.i_width = width;
    param.i_height = height;
    param.i_bframe = 0; //无b帧,首开,直播一般设为0
    param.rc.i_rc_method = X264_RC_ABR; //参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
    param.rc.i_bitrate = bitrate / 1000; //码率(比特率,单位Kbps)
    param.rc.i_vbv_max_bitrate = bitrate / 1000 * 1.2; //瞬时最大码率
    param.rc.i_vbv_buffer_size = bitrate / 1000; //设置了i_vbv_max_bitrate必须设置此参数，码率控制区大小,单位kbps
    param.i_fps_num = fps; // 帧率
    param.i_fps_den = 1; // 帧率的分母，1则一秒钟为一个单位

    /** 时间间隔 为了音视频同步
     *  param.time=20ms;
     *  param.i_timebase_num = 1s
     *  param.i_timebase_den=50
     *  1s/50 = 20ms
    */
    param.i_timebase_den = param.i_fps_num; // 时间基分子
    param.i_timebase_num = param.i_fps_den; // 时间基分母

    // param.pf_log = x264_log_default2;
    /**
     * b_vfr_input = 0 用帧率fps来控制码率
     * b_vfr_input = 1 用时间基和时间戳控制码率
     * */
    param.b_vfr_input = 0; // 用fps而不是时间戳来计算帧间距离
    param.i_keyint_max = fps * 2; //帧距离(关键帧)  2s一个关键帧
    /**
     * sps:序列参数集
     * pps:图像参数集
     * */
    param.b_repeat_headers = 1; // 是否复制sps和pps放在每个关键帧的前面 该参数设置是让每个关键帧(I帧)都附带sps/pps。
    param.i_threads = 1; //多线程，线程数，如果是0则自动多线程编码
    // 参数profile是编码器质量
    x264_param_apply_profile(&param, "baseline");
    videoCodec = x264_encoder_open(&param);

    // 编码出来的数据临时存储空间或位置，后面会转为I420
    pic_in = new x264_picture_t;
    x264_picture_alloc(pic_in, X264_CSP_I420, width, height);
}

void VideoChannelC::setVideoCallback(VideoCallback videoCallback) {
    this->videoCallback = videoCallback;
}

// 解码视频
void VideoChannelC::encodeData(int8_t *data){
    // 数据转换 格式：NV21 = >yuv420，数据：pic_in => img.plane
    // y数据
    memcpy(pic_in->img.plane[0], data, ySize);
    for (int i = 0; i < uvSize; ++i) {
        //v数据
        *(pic_in->img.plane[1] + i) = *(data + ySize + i * 2 + 1); // u,  1  3   5  7  9
        *(pic_in->img.plane[2] + i) = *(data + ySize + i * 2); // v,  0   2  4  6  8  10
    }
    // NALU单元
    x264_nal_t *pp_nal;
    // 编码出来有几个数据 （NALU单元数量）
    int pi_nal;
    x264_picture_t pic_out;
    // 将一张pic_in分解成n个NALU单元
    x264_encoder_encode(videoCodec, &pp_nal, &pi_nal, pic_in, &pic_out);
    int sps_len;
    int pps_len;
    uint8_t sps[100];
    uint8_t pps[100];
     /** 遍历每一个NALU单元
      * sps,pps是放在I帧单元里的；
      *【00 00 00 01 [67 sps 00 00 00 01 68 pps](NALU) ...】(流)
      * 分隔符后紧跟的第一个数字 67, “67 & 0x1F == 7”
      * 而NAL_SPS的 i_type值就是7，所以是根据67来判断sps的
      * 同理，68 & 0x1F == 8, 是PPS
    */
     for (int i = 0; i < pi_nal; ++i) {
        // 单独发送的
        if (pp_nal[i].i_type == NAL_SPS) {
            sps_len = pp_nal[i].i_payload - 4; // 减4是因为要减去关键帧前面的00 00 00 01
            memcpy(sps, pp_nal[i].p_payload + 4, sps_len);
        } else if (pp_nal[i].i_type == NAL_PPS) {
            pps_len = pp_nal[i].i_payload - 4;
            memcpy(pps, pp_nal[i].p_payload + 4, pps_len);
            // 单独发送sps,pps
            sendSpsPps(sps, pps, sps_len, pps_len);
        } else { // 非关键帧
            sendFrame(pp_nal[i].i_type, pp_nal[i].p_payload, pp_nal[i].i_payload);
        }
    }
}


void  VideoChannelC::sendSpsPps(uint8_t *sps, uint8_t *pps, int sps_len, int pps_len){
    int bodySize = 13 + sps_len + 3 + pps_len;
    RTMPPacket *packet = new RTMPPacket;
    RTMPPacket_Alloc(packet,bodySize);

    int i = 0;
    //固定头
    packet->m_body[i++] = 0x17; // 关键帧
    //类型
    packet->m_body[i++] = 0x00;
    //composition time 0x000000
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    //版本
    packet->m_body[i++] = 0x01;
    //编码规格
    packet->m_body[i++] = sps[1]; // prifile 也就是00 00 00 01后面的67
    packet->m_body[i++] = sps[2]; // 兼容性 00，就是67 64后面的00
    packet->m_body[i++] = sps[3]; // prifile编码级别 67 64 00 后面的33
    packet->m_body[i++] = 0xFF;   // 包长使用的字节数 0xFF固定

    //整个sps
    packet->m_body[i++] = 0xE1; // sps个数
    //sps长度
    packet->m_body[i++] = (sps_len >> 8) & 0xff;
    packet->m_body[i++] = sps_len & 0xff;
    memcpy(&packet->m_body[i], sps, sps_len);
    i += sps_len;

    //pps
    packet->m_body[i++] = 0x01; // 分隔符
    packet->m_body[i++] = (pps_len >> 8) & 0xff; //先取高8位再取低8位
    packet->m_body[i++] = (pps_len) & 0xff;
    memcpy(&packet->m_body[i], pps, pps_len);

    //视频
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nBodySize = bodySize;
    //随意分配一个管道（尽量避开rtmp.c中使用的）
    packet->m_nChannel = 10;
    //sps pps没有时间戳
    packet->m_nTimeStamp = 0;
    //不使用绝对时间
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;

    videoCallback(packet);
}