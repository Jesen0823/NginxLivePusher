package com.jesen.nginxlivepusher.av

import android.media.AudioFormat
import android.media.AudioRecord
import android.media.MediaRecorder
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class AudioChannel(livePusher: LivePusher) {
    private val mLivePusher: LivePusher
    private val audioRecord: AudioRecord
    private val inputSamples: Int
    private var channels = 2
    var channelConfig = 0
    var minBufferSize: Int
    private val executor: ExecutorService
    private var isLiving = false
    fun startLive() {
        isLiving = true
        executor.submit(AudioTeask())
    }

    fun setChannels(channels: Int) {
        this.channels = channels
    }

    fun release() {
        audioRecord.release()
    }

    internal inner class AudioTeask : Runnable {
      override  fun run() {
            audioRecord.startRecording()
            //    pcm  音频原始数据
            val bytes = ByteArray(inputSamples)
            while (isLiving) {
                val len: Int = audioRecord.read(bytes, 0, bytes.size)
                mLivePusher.native_pushAudio(bytes)
            }
        }
    }

    init {
        executor = Executors.newSingleThreadExecutor()
        mLivePusher = livePusher
        channelConfig = if (channels == 2) {
            AudioFormat.CHANNEL_IN_STEREO
        } else {
            AudioFormat.CHANNEL_IN_MONO
        }
        mLivePusher.native_setAudioEncInfo(44100, channels)
        //16 位 2个字节
        inputSamples = mLivePusher.getInputSamples() * 2
        //        minBufferSize
        minBufferSize = AudioRecord.getMinBufferSize(
            44100,
            channelConfig, AudioFormat.ENCODING_PCM_16BIT
        ) * 2
        audioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC,
            44100,
            channelConfig,
            AudioFormat.ENCODING_PCM_16BIT,
            if (minBufferSize < inputSamples) inputSamples else minBufferSize
        )
    }

}