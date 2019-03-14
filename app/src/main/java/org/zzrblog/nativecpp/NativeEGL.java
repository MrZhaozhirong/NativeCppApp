package org.zzrblog.nativecpp;

import android.view.Surface;

/**
 * Created by zzr on 2019/3/14.
 */
public class NativeEGL {

    static {
        System.loadLibrary("native-egl");
    }

    public native void onSurfaceCreate(Surface surface);

    public native void onSurfaceChange(int width, int height);

    public native void onSurfaceDestory();



}
