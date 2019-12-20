package org.zzrblog.gpufilter;

import android.content.Context;
import android.view.Surface;

/**
 * Created by zzr on 2019/11/27.
 */
class GpuFilterRender {
    static {
        System.loadLibrary("gpu-filter");
    }

    private Context ctx;
    GpuFilterRender(Context context) {
        ctx = context;
    }

    public native void onSurfaceCreate(Surface surface);

    public native void onSurfaceChange(Surface surface, int width, int height);

    public native void onSurfaceDestroy(Surface surface);

    // 发送视频nv21数据
    public native void feedVideoData(byte[] data,int width,int height);
    // 发送音频 pcm数据
    //public native void feedAudioData(byte[] data);

    /**
     * 设置摄像头角度和方向
     * @param rotation 角度
     * @param flipHorizontal 是否水平翻转
     * @param flipVertical 是否垂直翻转
     */
    public native void setRotationCamera(final int rotation, final boolean flipHorizontal,
                                         final boolean flipVertical);


    public native void setFilterType(int typeId);
    public native void adjustFilterValue(int value,int max);
}
