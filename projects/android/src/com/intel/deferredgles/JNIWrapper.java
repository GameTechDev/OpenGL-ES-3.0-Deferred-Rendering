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
    public static native void init_asset_manager(AssetManager asset_manager);
    public static native void frame();
}
