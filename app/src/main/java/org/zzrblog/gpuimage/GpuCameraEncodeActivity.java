package org.zzrblog.gpuimage;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;

import org.zzrblog.nativecpp.R;

/**
 * Created by zzr on 2019/11/19.
 */
public class GpuCameraEncodeActivity extends Activity{


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gpu_camera_encode);
    }


}
