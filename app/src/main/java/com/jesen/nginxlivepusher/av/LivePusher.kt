package com.jesen.nginxlivepusher.av

import android.app.Activity
import android.view.SurfaceHolder

class LivePusher(
    activity: Activity, width: Int, height: Int, bitrate: Int,
    fps: Int, cameraId: Int
) {
    private val audioChannel: AudioChannel
    private val videoChannel: VideoChannel

    companion object {
        init {
            System.loadLibrary("nginxlivepusher")
        }
    }

    init {
        native_init()
        videoChannel = VideoChannel(this, activity, width, height, bitrate, fps, cameraId)
        audioChannel = AudioChannel(this)
    }

    fun setPreviewDisplay(surfaceHolder: SurfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder)
    }

    fun switchCamera() {
        videoChannel.switchCamera()
    }

    fun startLive(address: String) {
        native_start(address)
        videoChannel.startLive()
        audioChannel.startLive()
    }


    fun release() {
        audioChannel.release()
        videoChannel.release()
        native_release()
    }

    external fun native_init()
    external fun native_setVideoEncInfo(width: Int, height: Int, fps: Int, bitrate: Int)
    external fun native_start(path: String)
    external fun native_pushVideo(data: ByteArray)
    external fun native_pushAudio(bytes: ByteArray)
    // 音频采样位数，声道数
    external fun native_setAudioEncInfo(sampleRateInHz: Int, channels: Int)

    external fun getInputSamples(): Int

    external fun native_release()
}