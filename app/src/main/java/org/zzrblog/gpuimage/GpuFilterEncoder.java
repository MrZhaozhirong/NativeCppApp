package org.zzrblog.gpuimage;

import android.content.Context;
import android.view.Surface;

/**
 * Created by zzr on 2019/11/27.
 */
class GpuFilterEncoder {
    static {
        System.loadLibrary("gpu-filter");
    }

    private Context ctx;
    GpuFilterEncoder(Context context) {
        ctx = context;
    }


    public native void onSurfaceCreate(Surface surface);

    public native void onSurfaceChange(Surface surface, int width, int height);

    public native void onSurfaceDestroy(Surface surface);

    /**
     * 发送视频数据
     * @param data nv21数据
     */
    public native void feedVideoData(byte[] data);
    /**
     * 发送音频数据
     * @param data pcm数据
     * @param len 数据长度
     */
    // public native void feedAudioData(byte[] data, int len);
}
