package com.jesen.nginxlivepusher.av

import android.app.Activity
import android.hardware.Camera

class VideoChannel(
    livePusher: LivePusher,
    activity: Activity?,
    width: Int,
    height: Int,
    private val mBitrate: Int,
    private val mFps: Int,
    cameraId: Int
) : Camera.PreviewCallback, CameraHelper.OnChangedSizeListener {
    override fun onPreviewFrame(p0: ByteArray?, p1: Camera?) {
    }

    override fun onChanged(w: Int, h: Int) {

    }

}