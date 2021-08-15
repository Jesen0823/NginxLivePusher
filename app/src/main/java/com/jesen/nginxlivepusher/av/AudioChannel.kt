package com.jesen.nginxlivepusher.av

import android.annotation.SuppressLint
import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

@SuppressLint("MissingPermission")
class AudioChannel(livePusher: LivePusher) {
    private var mLivePusher: LivePusher = livePusher
    private var audioRecord: AudioRecord
    private var inputSamples: Int = mLivePusher.getInputSamples() * 2
    private var channels: Int
    private var channelConfig = 0
    private var minBufferSize: Int
    private var executor: ExecutorService = Executors.newSingleThreadExecutor()
    private var isLiving = false

    init {
        channels = 2
        channelConfig = if (channels == 2) {
            AudioFormat.CHANNEL_IN_STEREO
        } else {
            AudioFormat.CHANNEL_IN_MONO
        }
        // 设置采样率，声道数
        mLivePusher.native_setAudioEncInfo(44100, channels)

        minBufferSize = AudioRecord.getMinBufferSize(
            44100,
            channelConfig, AudioFormat.ENCODING_PCM_16BIT
        ) * 2

        // 最后一个参数：音频缓冲区大小
        audioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC,
            44100,
            channelConfig,
            AudioFormat.ENCODING_PCM_16BIT,
            if (minBufferSize < inputSamples) inputSamples else minBufferSize
        )

    }

    fun startLive() {
        isLiving = true
        executor.submit(AudioTask())
    }

    fun setChannels(channels: Int) {
        this.channels = channels
    }

    fun release() {
        audioRecord.release()
    }

    internal inner class AudioTask : Runnable {
        override fun run() {
            audioRecord.startRecording()
            // pcm音频原始数据
            val bytes = ByteArray(inputSamples)
            while (isLiving) {
                val len: Int = audioRecord.read(bytes, 0, bytes.size)
                mLivePusher.native_pushAudio(bytes)
            }
        }
    }
}