package org.zzrblog.gpufilter;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.lang.ref.WeakReference;

/**
 * Created by zzr on 2019/11/27.
 * CFE : Camera Filter Encode
 * 这些逻辑代码，如果不嫌弃混乱，可以直接写在CameraFilterEncodeActivity.
 */
public class CFEScheduler implements Camera.PreviewCallback, SurfaceHolder.Callback {
    private static final String TAG = "CFEScheduler";
    private WeakReference<Activity> mActivityWeakRef;
    private GpuFilterRender mGpuFilterRender;

    /*Filter相关*/
    public void setFilterType(int typeId) {
        if(mGpuFilterRender!=null)
            mGpuFilterRender.setFilterType(typeId);
    }
    private FilterType supportFilters;
    public String[] getSupportedFiltersName() {
        if( supportFilters ==null ) {
            supportFilters = new FilterType();
            supportFilters.addFilter("Normal(无特效)", FilterType.FILTER_TYPE_NORMAL);
            supportFilters.addFilter("Color-Invert(黑白取反)", FilterType.FILTER_TYPE_COLOR_INVERT);
            supportFilters.addFilter("Pixelation(马赛克)", FilterType.FILTER_TYPE_PIXELATION);
            supportFilters.addFilter("Contrast(对比度)", FilterType.FILTER_TYPE_CONTRAST);
            supportFilters.addFilter("Brightness(亮度)", FilterType.FILTER_TYPE_BRIGHTNESS);
            supportFilters.addFilter("Hue(色调)", FilterType.FILTER_TYPE_HUE);
            supportFilters.addFilter("Exposure(曝光度)", FilterType.FILTER_TYPE_EXPOSURE);
            supportFilters.addFilter("Saturation(饱和度)", FilterType.FILTER_TYPE_SATURATION);
            supportFilters.addFilter("Sharpen(锐化)", FilterType.FILTER_TYPE_SHARPEN);
            supportFilters.addFilter("BoxBlur(均值滤波)", FilterType.FILTER_TYPE_BOXBLUR);
        }
        return supportFilters.names.toArray(new String[supportFilters.names.size()]);
    }
    public int getSupportedFilterTypeID(String name) {
        if(supportFilters!=null) {
            try{// 防止空指针|越界|查询失败
                int position = supportFilters.names.indexOf(name);
                return supportFilters.filters.get(position);
            }catch (Exception e){
                return 0;
            }
        }
        return 0;
    }
    public int getSupportedFilterTypeID(int index) {
        if(supportFilters!=null) {
            try{// 防止空指针|越界|查询失败
                return supportFilters.filters.get(index);
            }catch (Exception e){
                return 0;
            }
        }
        return 0;
    }
    public void adjustFilterValue(int value, int max) {
        if(mGpuFilterRender!=null)
            mGpuFilterRender.adjustFilterValue(value, max);
    }


    /*Camera SurfaceView相关*/
    CFEScheduler(Activity activity, SurfaceView view) {
        mActivityWeakRef = new WeakReference<>(activity);
        mGpuFilterRender = new GpuFilterRender(activity);

        view.getHolder().setFormat(PixelFormat.RGBA_8888);
        view.getHolder().addCallback(this);
    }

    public void onResume() {
        Log.d(TAG, "onResume ...");
        setUpCamera(mCurrentCameraId);
    }

    public void onPause() {
        Log.d(TAG, "onPause ...");
        releaseCamera();
    }

    public void switchCamera() {
        releaseCamera();
        mCurrentCameraId = (mCurrentCameraId + 1) % Camera.getNumberOfCameras();
        setUpCamera(mCurrentCameraId);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        // if(mCameraTexture!=null){
        //     mCameraTexture.updateTexImage();
        // }
        if( mGpuFilterRender!=null){
            final Camera.Size previewSize = camera.getParameters().getPreviewSize();
            mGpuFilterRender.feedVideoData(data.clone(), previewSize.width, previewSize.height);
        }
    }

    private SurfaceTexture mCameraTexture = null;
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated ... ");
        try {
            int[] textures = new int[1];
            GLES20.glGenTextures(1, textures, 0);
            mCameraTexture = new SurfaceTexture(textures[0]);
            mCameraInstance.setPreviewTexture(mCameraTexture);
            mCameraInstance.setPreviewCallback(this);
            mCameraInstance.startPreview();
        } catch (Exception e) {
            e.printStackTrace();
        }
        mGpuFilterRender.onSurfaceCreate(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged ... ");
        mGpuFilterRender.onSurfaceChange(holder.getSurface(), width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed ... ");
        mGpuFilterRender.onSurfaceDestroy(holder.getSurface());
        if( mCameraTexture!=null){
            mCameraTexture.release();
        }
    }

    private int mCurrentCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private Camera mCameraInstance;

    private void setUpCamera(final int id) {
        mCameraInstance = getCameraInstance(id);
        Camera.Parameters parameters = mCameraInstance.getParameters();
        parameters.setPreviewFormat(ImageFormat.NV21);
        // NOTE!!! TODO
        // adjust by getting supportedPreviewSizes and then choosing
        // the best one for screen size (best fill screen)
        if (parameters.getSupportedFocusModes().contains(
                Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }
        mCameraInstance.setParameters(parameters);

        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        Camera.getCameraInfo(mCurrentCameraId, cameraInfo);

        Activity activity = mActivityWeakRef.get();
        int orientation = getCameraDisplayOrientation(activity, cameraInfo);
        Log.i(TAG, "getCameraDisplayOrientation : "+orientation);
        boolean flipHorizontal = cameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT;
        mGpuFilterRender.setRotationCamera(orientation, flipHorizontal, false);
    }

    private int getCameraDisplayOrientation(final Activity activity,
                                            @NonNull Camera.CameraInfo info) {
        if(activity == null) return 0;
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        return result;
    }

    /** A safe way to get an instance of the Camera object. */
    private Camera getCameraInstance(final int id) {
        Camera c = null;
        try {
            c = Camera.open(id);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return c;
    }

    private void releaseCamera() {
        mCameraInstance.setPreviewCallback(null);
        mCameraInstance.release();
        mCameraInstance = null;
    }

}
