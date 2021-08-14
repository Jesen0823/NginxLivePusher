package com.jesen.nginxlivepusher.util

import android.util.Log

class OLog {
    companion object{
        fun d(msg:String){
            Log.d("LivePusher",msg)
        }
        fun e(msg:String){
            Log.e("LivePusher",msg)
        }
    }
}