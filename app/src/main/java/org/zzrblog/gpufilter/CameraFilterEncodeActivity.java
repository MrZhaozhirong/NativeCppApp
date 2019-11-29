package org.zzrblog.gpufilter;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.SurfaceView;
import android.view.View;

import org.zzrblog.nativecpp.R;


/**
 * Created by zzr on 2019/11/19.
 */
public class CameraFilterEncodeActivity extends Activity {

    private CFEScheduler cfeScheduler;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_filter_encode);

        SurfaceView surfaceView = findViewById(R.id.camera_view);
        findViewById(R.id.img_switch_camera)
                .setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                cfeScheduler.switchCamera();
            }
        });

        if( cfeScheduler==null)
            cfeScheduler = new CFEScheduler(this, surfaceView);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if( cfeScheduler!=null) {
            cfeScheduler.onResume();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if( cfeScheduler!=null) {
            cfeScheduler.onPause();
        }
    }

}
