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
