package com.jesen.nginxlivepusher.av

import android.app.Activity
import android.hardware.Camera
import android.view.SurfaceHolder


class CameraHelper(activity: Activity, cameraId: Int, width: Int, height: Int) :
    SurfaceHolder.Callback, Camera.PreviewCallback {
    override fun surfaceCreated(p0: SurfaceHolder) {
        
    }

    override fun surfaceChanged(p0: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        
    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {
        
    }

    override fun onPreviewFrame(p0: ByteArray?, p1: Camera?) {
        
    }

    interface OnChangedSizeListener {
        fun onChanged(w: Int, h: Int)
    }
}