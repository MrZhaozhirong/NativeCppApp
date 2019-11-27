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
}
