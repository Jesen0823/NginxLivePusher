package com.jesen.nginxlivepusher

import android.Manifest
import android.hardware.Camera
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.jesen.nginxlivepusher.av.LivePusher
import com.jesen.nginxlivepusher.databinding.ActivityMainBinding
import androidx.core.app.ActivityCompat

import android.widget.Toast

import android.content.pm.PackageManager

import androidx.core.content.ContextCompat

private const val MY_PERMISSIONS_CODE = 1

class MainActivity : AppCompatActivity() {

    private val RTMP_ADDRES = "rtmp://47.107.132.117/myapp"

    private lateinit var binding: ActivityMainBinding
    private lateinit var livePusher: LivePusher

    private var permissions = arrayOf<String>(
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
    )
    private var mPermissionList: ArrayList<String> = ArrayList()

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
        // 请求权限
        requestPermissionsAll()
    }

    private fun requestPermissionsAll() {
        mPermissionList.clear()
        for (i in permissions.indices) {
            if (ContextCompat.checkSelfPermission(
                    this@MainActivity,
                    permissions[i]
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                mPermissionList.add(permissions[i])
            }
        }
        if (mPermissionList.isEmpty()) {
            //未授予的权限为空，表示都授予了
            Toast.makeText(this@MainActivity, "已经授权", Toast.LENGTH_LONG).show()
        } else { //请求权限方法
            val permissions = mPermissionList.toTypedArray() //将List转为数组
            ActivityCompat.requestPermissions(
                this@MainActivity,
                permissions,
                MY_PERMISSIONS_CODE
            )
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String?>,
        grantResults: IntArray
    ) {
    if (requestCode == MY_PERMISSIONS_CODE) {
            for (i in grantResults.indices) {
                if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                    //判断是否勾选禁止后不再询问
                    val showRequestPermission = ActivityCompat.shouldShowRequestPermissionRationale(
                        this@MainActivity,
                        permissions[i]!!
                    )
                    if (showRequestPermission) {
                        requestPermissionsAll()
                    }
                }
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    override fun onDestroy() {
        super.onDestroy()
        livePusher.release()
    }
}