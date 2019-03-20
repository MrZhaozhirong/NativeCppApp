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

    public native void onSurfaceDestroy();


    public native void handleMultiTouch(float distance);

    public native void handleTouchDown(float x, float y);

    public native void handleTouchDrag(float x, float y);

    public native void handleTouchUp(float x, float y);

}
