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

    }

    fun switchCamera() {

    }

    fun startLive(address: String) {

    }

    fun release() {
    }

    external fun native_init()

}