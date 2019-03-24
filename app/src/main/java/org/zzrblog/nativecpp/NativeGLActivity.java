package org.zzrblog.nativecpp;

import android.graphics.PixelFormat;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

/**
 * Created by zzr on 2019/3/12.
 */
public class NativeGLActivity extends AppCompatActivity {

    private NativeEGL nativeEGL;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_gl);
        SurfaceView surfaceview = findViewById(R.id.easy_surface_view);
        surfaceview.getHolder().setFormat(PixelFormat.RGBA_8888);

        nativeEGL = new NativeEGL(this);

        surfaceview.getHolder().addCallback(new SurfaceHolder.Callback() {

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                nativeEGL.onSurfaceCreate(holder.getSurface());
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                nativeEGL.onSurfaceChange(width, height);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                nativeEGL.onSurfaceDestroy();
            }
        });


        surfaceview.setOnTouchListener(new GLViewTouchListener());
    }

    private float spacing(MotionEvent event) {
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return (float) Math.sqrt(x * x + y * y);
    }

    private class GLViewTouchListener implements View.OnTouchListener {
        private int mode = 0;
        private float oldDist;

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            // -------------判断多少个触碰点---------------------------------
            switch (event.getAction() & MotionEvent.ACTION_MASK ){
                case MotionEvent.ACTION_DOWN:
                    mode = 1;
                    break;
                case MotionEvent.ACTION_UP:
                    mode = 0;
                    break;
                case MotionEvent.ACTION_POINTER_UP:
                    mode -= 1;
                    break;
                case MotionEvent.ACTION_POINTER_DOWN:
                    oldDist = spacing(event);
                    mode += 1;
                    break;
            }

            if(event.getAction() == MotionEvent.ACTION_DOWN){
                if(mode == 1) {
                    final float x = event.getX();
                    final float y = event.getY();
                    nativeEGL.handleTouchDown(x, y);
                }

            }else if(event.getAction() ==MotionEvent.ACTION_MOVE){
                if (mode == 1) {
                    final float x = event.getX();
                    final float y = event.getY();
                    nativeEGL.handleTouchDrag(x,y);
                }
                if (mode == 2) {
                    //双指操作
                    float newDist = spacing(event);
                    if ( (newDist > oldDist + 20) || (newDist < oldDist - 20) ) {
                        final float distance = newDist - oldDist;
                        nativeEGL.handleMultiTouch(distance);
                        oldDist = newDist;
                    }
                }
            }else if(event.getAction() == MotionEvent.ACTION_UP){
                if (mode == 1) {
                    final float x = event.getX();
                    final float y = event.getY();
                    nativeEGL.handleTouchUp(x,y);
                }
            }

            return true;
        }
    }
}
