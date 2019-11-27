package org.zzrblog;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.Toast;

import org.zzrblog.gpuimage.CameraFilterEncodeActivity;
import org.zzrblog.nativecpp.NativeGLActivity;
import org.zzrblog.nativecpp.R;

/**
 * Created by zzr on 2019/3/26.
 */
public class PermissionActivity extends Activity {

    private static final int ACTION_REQUEST_PERMISSIONS = 0x1154;

    private static final String[] NEEDED_PERMISSIONS = new String[]{
            android.Manifest.permission.READ_EXTERNAL_STORAGE,
            android.Manifest.permission.WRITE_EXTERNAL_STORAGE,
            android.Manifest.permission.READ_PHONE_STATE,
            android.Manifest.permission.CAMERA,
            android.Manifest.permission.RECORD_AUDIO
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_permission);
    }



    public void clickOnNativeEGL(View view) {
        startActivity(new Intent(PermissionActivity.this, NativeGLActivity.class));
    }

    public void clickOnCameraFilterEncode(View view) {
        startActivity(new Intent(PermissionActivity.this, CameraFilterEncodeActivity.class));
    }



    @Override
    protected void onResume() {
        super.onResume();
        if (!checkPermissions(NEEDED_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, NEEDED_PERMISSIONS, ACTION_REQUEST_PERMISSIONS);
            return;
        }
    }

    private boolean checkPermissions(String[] neededPermissions) {
        if (neededPermissions == null || neededPermissions.length == 0) {
            return true;
        }
        boolean allGranted = true;
        for (String neededPermission : neededPermissions) {
            allGranted &= ContextCompat.checkSelfPermission(this, neededPermission) == PackageManager.PERMISSION_GRANTED;
        }
        return allGranted;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == ACTION_REQUEST_PERMISSIONS) {
            boolean isAllGranted = true;
            for (int grantResult : grantResults) {
                isAllGranted &= (grantResult == PackageManager.PERMISSION_GRANTED);
            }
            if (!isAllGranted) {
                Toast.makeText(this, "Need Permissions to RunApp Normally.", Toast.LENGTH_SHORT).show();
            }
        }
    }

}
