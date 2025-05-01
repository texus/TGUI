package com.raylib.raymob;

import android.view.View;
import android.app.NativeActivity;
import android.view.WindowManager.LayoutParams;

public class DisplayManager {

    NativeActivity activity;

    public DisplayManager(android.content.Context context) {
        activity = (NativeActivity)(context);
        if (BuildConfig.FEATURE_DISPLAY_KEEP_ON) {
            keepScreenOn(true);
        }
        if (BuildConfig.FEATURE_DISPLAY_IMMERSIVE) {
            setImmersiveMode();
        }
        if (BuildConfig.FEATURE_DISPLAY_INTO_CUTOUT) {
            renderIntoCutoutArea();
        }
    }

    public void keepScreenOn(boolean keepOn) {
        if (keepOn) {
            activity.getWindow().addFlags(LayoutParams.FLAG_KEEP_SCREEN_ON);
        } else {
            activity.getWindow().clearFlags(LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    public void setImmersiveMode() {
        activity.getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN              |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION         |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY        |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN       |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION  |
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        );
    }

    public void renderIntoCutoutArea() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
            LayoutParams lp = activity.getWindow().getAttributes();
            lp.layoutInDisplayCutoutMode = LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            activity.getWindow().setAttributes(lp);
        }
    }

}
