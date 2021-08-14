package com.jesen.nginxlivepusher

import android.hardware.Camera
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.jesen.nginxlivepusher.av.LivePusher
import com.jesen.nginxlivepusher.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private val RTMP_ADDRES = "rtmp://47.107.132.117/myapp"

    private lateinit var binding: ActivityMainBinding
    private lateinit var livePusher: LivePusher

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        livePusher = LivePusher(this, 800,400, 800_000,10, Camera.CameraInfo.CAMERA_FACING_BACK)
        // 设置摄像头预览界面
        livePusher.setPreviewDisplay(binding.surfaceView.holder)

        binding.switchCamera.setOnClickListener {
            livePusher.switchCamera()
        }
        binding.startLive.setOnClickListener {
            livePusher.startLive(RTMP_ADDRES)
        }

        binding.stopLive.setOnClickListener {
            livePusher.release()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        livePusher.release()
    }
}