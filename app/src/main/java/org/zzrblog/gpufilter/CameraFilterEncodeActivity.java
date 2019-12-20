package org.zzrblog.gpufilter;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.SeekBar;
import android.widget.Spinner;

import org.zzrblog.nativecpp.R;


/**
 * Created by zzr on 2019/11/19.
 */
public class CameraFilterEncodeActivity extends Activity {
    private static final String TAG = "CFEScheduler";
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

        initFilterSpinner();
        initFilterAdjuster();
    }

    SeekBar filterAdjuster;
    private void initFilterAdjuster() {
        filterAdjuster = findViewById(R.id.value_seek_bar);
        filterAdjuster.setMax(100);
        filterAdjuster.setProgress(0);
        filterAdjuster.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                cfeScheduler.adjustFilterValue(progress, seekBar.getMax());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) { }
        });
    }

    private void initFilterSpinner() {
        String[] mItems = cfeScheduler.getSupportedFiltersName();
        //String[] mItems = this.getResources().getStringArray(R.array.filter_name);
        Spinner filterSpinner = findViewById(R.id.filter_spinner);
        ArrayAdapter<String> adapter=new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, mItems);
        filterSpinner.setAdapter(adapter);
        filterSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                // 更新复原Filter.Adjuster
                filterAdjuster.setProgress(0);
                // 正常是通过getSupportedFilterTypeID(name)查询typeId
                // 这里的position更好是names[]的索引，所以可以直接通过index查询typeId了
                int typeId = cfeScheduler.getSupportedFilterTypeID(position);
                cfeScheduler.setFilterType(typeId);
            }

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
