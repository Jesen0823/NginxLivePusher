package com.jesen.nginxlivepusher.av

import android.app.Activity
import android.graphics.ImageFormat
import android.hardware.Camera
import android.hardware.Camera.CameraInfo
import android.hardware.Camera.PreviewCallback
import android.util.Log
import android.view.Surface
import android.view.SurfaceHolder
import com.jesen.nginxlivepusher.util.OLog
import java.lang.Exception


class CameraHelper(activity: Activity, cameraId: Int, width: Int, height: Int) :
    SurfaceHolder.Callback, Camera.PreviewCallback {
    
    private val TAG = "CameraHelper"
    private  var mActivity = activity
    private  var mHeight = height
    private  var mWidth = width
    private  var mCameraId = cameraId
    
    private var mCamera:Camera? = null
    private var buffer: ByteArray? = null
    private lateinit var mSurfaceHolder: SurfaceHolder
    private var mPreviewCallback: PreviewCallback? = null
    private var mRotation = 0
    private var mOnChangedSizeListener: OnChangedSizeListener? = null
    
    lateinit var bytes: ByteArray    


    override fun surfaceCreated(p0: SurfaceHolder) {
        
    }

    override fun surfaceChanged(p0: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
        //释放摄像头
        stopPreview()
        //开启摄像头
        startPreview()
    }

    override fun surfaceDestroyed(p0: SurfaceHolder) {
        
    }

    override fun onPreviewFrame(data: ByteArray, camera: Camera) {
        when (mRotation) {
            Surface.ROTATION_0 -> rotation90(data)
            Surface.ROTATION_90 -> {/*横屏 左边是头部(home键在右边)*/
            }
            Surface.ROTATION_270 -> {/* 横屏，头部在右边*/
            }
        }
        // data数据依然是倒的
        // data数据依然是倒的
        mPreviewCallback?.onPreviewFrame(bytes, camera)
        camera.addCallbackBuffer(buffer)
    }

    private fun rotation90(data: ByteArray) {
        var index = 0
        val ySize = mWidth * mHeight
        //u和v
        val uvHeight = mHeight / 2
        //后置摄像头顺时针旋转90度
        if (mCameraId == CameraInfo.CAMERA_FACING_BACK) {
            //将y的数据旋转之后 放入新的byte数组
            for (i in 0 until mWidth) {
                for (j in mHeight - 1 downTo 0) {
                    bytes[index++] = data[mWidth * j + i]
                }
            }

            //每次处理两个数据
            var i = 0
            while (i < mWidth) {
                for (j in uvHeight - 1 downTo 0) {
                    // v
                    bytes[index++] = data[ySize + mWidth * j + i]
                    // u
                    bytes[index++] = data[ySize + mWidth * j + i + 1]
                }
                i += 2
            }
        } else {
            //逆时针旋转90度
            for (i in 0 until mWidth) {
                var nPos = mWidth - 1
                for (j in 0 until mHeight) {
                    bytes[index++] = data[nPos - i]
                    nPos += mWidth
                }
            }
            //u v
            var i = 0
            while (i < mWidth) {
                var nPos = ySize + mWidth - 1
                for (j in 0 until uvHeight) {
                    bytes[index++] = data[nPos - i - 1]
                    bytes[index++] = data[nPos - i]
                    nPos += mWidth
                }
                i += 2
            }
        }
    }
    
    fun switchCamera() {
        mCameraId = if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            Camera.CameraInfo.CAMERA_FACING_FRONT
        } else {
            Camera.CameraInfo.CAMERA_FACING_BACK
        }
        stopPreview()
        startPreview()
    }

    private fun test(){
        val previewSizes: List<Camera.Size> = mCamera!!.parameters.supportedPreviewSizes
        for (i in previewSizes.indices) {
            val psize: Camera.Size = previewSizes[i]
            Log.i(
                TAG + "initCamera",
                "PreviewSize,width: " + psize.width.toString() + " height: " + psize.height
            )
        }
    }

    private fun startPreview() {
        mCamera?.let {
            try {
                //获得camera对象
                mCamera = Camera.open(mCameraId)
                test()
                //配置camera的属性
                val parameters = it.getParameters()
                //设置预览数据格式为nv21
                parameters.previewFormat = ImageFormat.NV21
                //这是摄像头宽、高
                setPreviewSize(parameters)
                // 设置摄像头 图像传感器的角度、方向
                setPreviewOrientation(parameters)
                it.setParameters(parameters)
                buffer = ByteArray(mWidth * mHeight * 3 / 2)
                bytes = ByteArray(buffer!!.size)
                //数据缓存区
                it.addCallbackBuffer(buffer)
                it.setPreviewCallbackWithBuffer(this)
                //设置预览画面
                it.setPreviewDisplay(mSurfaceHolder)
                it.startPreview()
            } catch (ex: Exception) {
                ex.printStackTrace()
            }
        }
    }

    private fun setPreviewOrientation(parameters: Camera.Parameters?) {
        val info = CameraInfo()
        Camera.getCameraInfo(mCameraId, info)
        mRotation = mActivity.windowManager.defaultDisplay.rotation
        var degrees = 0
        when (mRotation) {
            Surface.ROTATION_0 -> {
                degrees = 0
                mOnChangedSizeListener!!.onChanged(mHeight, mWidth)
            }
            Surface.ROTATION_90 -> {
                degrees = 90
                mOnChangedSizeListener!!.onChanged(mWidth, mHeight)
            }
            Surface.ROTATION_270 -> {
                degrees = 270
                mOnChangedSizeListener!!.onChanged(mWidth, mHeight)
            }
        }
        var result: Int
        if (info.facing == CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360
            result = (360 - result) % 360 // compensate the mirror
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360
        }
        //设置角度
        //设置角度
        mCamera!!.setDisplayOrientation(result)
    }


    private fun setPreviewSize(parameters: Camera.Parameters?) {
        //获取摄像头支持的宽、高

        //获取摄像头支持的宽、高
        val supportedPreviewSizes = parameters!!.supportedPreviewSizes
        var size = supportedPreviewSizes[0]
        OLog.d(TAG+",支持 " + size.width + "x" + size.height)
        //选择一个与设置的差距最小的支持分辨率
        // 10x10 20x20 30x30
        // 12x12
        //选择一个与设置的差距最小的支持分辨率
        // 10x10 20x20 30x30
        // 12x12
        var m = Math.abs(size.height * size.width - mWidth * mHeight)
        supportedPreviewSizes.removeAt(0)
        val iterator: Iterator<Camera.Size> = supportedPreviewSizes.iterator()
        //遍历
        //遍历
        while (iterator.hasNext()) {
            val next = iterator.next()
            OLog.d(TAG+", 支持 " + next.width + "x" + next.height)
            val n = Math.abs(next.height * next.width - mWidth * mHeight)
            if (n < m) {
                m = n
                size = next
            }
        }
        mWidth = size.width
        mHeight = size.height
        parameters!!.setPreviewSize(mWidth, mHeight)
        OLog.d(TAG+", 设置预览分辨率 width:" + size.width + " height:" + size.height)

    }

    private fun stopPreview() {
        mCamera?.let {
            //预览数据回调接口
            setPreviewCallback(null)
            //停止预览
            stopPreview()
            //释放摄像头
            release()
            mCamera = null
        }
    }

    fun setPreviewCallback(previewCallback: PreviewCallback?) {
        mPreviewCallback = previewCallback
    }

    fun setPreviewDisplay(surfaceHolder: SurfaceHolder) {
        mSurfaceHolder = surfaceHolder
        mSurfaceHolder.addCallback(this)
    }

    fun release() {
        mSurfaceHolder.removeCallback(this)
        stopPreview()
    }

    fun setOnChangedSizeListener(listener: OnChangedSizeListener) {
        mOnChangedSizeListener = listener
    }

    interface OnChangedSizeListener {
        fun onChanged(w: Int, h: Int)
    }
}