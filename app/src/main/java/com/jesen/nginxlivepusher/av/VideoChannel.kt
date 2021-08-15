package com.jesen.nginxlivepusher.av

import android.app.Activity
import android.hardware.Camera
import android.util.Log
import android.view.SurfaceHolder
import com.jesen.nginxlivepusher.util.OLog

class VideoChannel(
    livePusher: LivePusher,
    activity: Activity,
    width: Int,
    height: Int,
    private val mBitrate: Int,
    private val mFps: Int,
    cameraId: Int
) : Camera.PreviewCallback, CameraHelper.OnChangedSizeListener {
    private val cameraHelper: CameraHelper = CameraHelper(activity, cameraId, width, height)
    private var isLiving = false
    private var livePusher: LivePusher = livePusher

    companion object {
        private const val TAG = "VideoChannel"
    }

    init {
        cameraHelper.setPreviewCallback(this)
        cameraHelper.setOnChangedSizeListener(this)
    }

    //data   nv21
    override fun onPreviewFrame(data: ByteArray?, camera: Camera?) {
        OLog.d(TAG+", onPreviewFrame: ")
        if (isLiving) {
            if (data != null) {
                livePusher.native_pushVideo(data)
            }
        }
    }

    override fun onChanged(w: Int, h: Int) {
        livePusher.native_setVideoEncInfo(w, h, mFps, mBitrate)
    }

    fun switchCamera() {
        cameraHelper.switchCamera()
    }

    fun setPreviewDisplay(surfaceHolder: SurfaceHolder?) {
        surfaceHolder?.let { cameraHelper.setPreviewDisplay(it) }
    }

    fun startLive() {
        isLiving = true
    }

    fun release() {
        cameraHelper.release()
    }
}