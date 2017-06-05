////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

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
