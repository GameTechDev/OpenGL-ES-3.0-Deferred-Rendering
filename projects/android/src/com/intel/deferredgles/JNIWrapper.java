package com.intel.deferredgles;

import android.content.res.AssetManager;

/** JNI wrapper
 */
public class JNIWrapper
{
    static {
        System.loadLibrary("androidinterface");
    }
    public static native void init(int width, int height);
    public static native void resize(int width, int height);
    public static native void init_asset_manager(AssetManager asset_manager);
    public static native void frame();

    public static native void touch_down(int index, float x, float y);
    public static native void touch_up(int index, float x, float y);
    public static native void touch_move(int index, float x, float y);
}
