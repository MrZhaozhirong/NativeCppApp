package org.zzrblog.gpufilter;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import org.zzrblog.nativecpp.R;


/**
 * Created by zzr on 2019/11/19.
 */
public class CameraFilterEncodeActivity extends Activity {

    private CFEScheduler cfeScheduler;
    private Spinner filterSpinner;

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

        initFilterSpinner();
    }

    private void initFilterSpinner() {
        //String[] mItems = cfeScheduler.getSupportedFilter();
        String[] mItems = this.getResources().getStringArray(R.array.filter_name);
        filterSpinner = findViewById(R.id.filter_spinner);
        ArrayAdapter<String> adapter=new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, mItems);
        filterSpinner.setAdapter(adapter);
        filterSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) { }

            @Override
            public void onNothingSelected(AdapterView<?> parent) { }
        });
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
