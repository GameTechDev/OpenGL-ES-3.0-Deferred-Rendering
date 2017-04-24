/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

package com.intel.deferredgles;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.widget.Toast;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.MotionEvent;
import android.opengl.GLSurfaceView.EGLContextFactory;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGL10;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class DeferredGLES extends Activity
{
    private AssetManager _asset_manager;
    private GLSurfaceView _view;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        _view = new GLSurfaceView(this);
        _view.setEGLContextClientVersion(2);
        _view.setEGLConfigChooser(8,8,8,8,16,0);
        _view.setRenderer(new Renderer());
        setContentView(_view);

        /* Load asset manager */
        _asset_manager = getAssets();
        JNIWrapper.init_asset_manager(_asset_manager);
    }

    @Override protected void onPause()
    {
        super.onPause();
        _view.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        _view.onResume();
    }

    /** Input handling
     */
    @Override
    public boolean onTouchEvent( MotionEvent event ) {
        int index = event.getActionIndex();
        int pointerId = event.getPointerId(index);
        int action = event.getActionMasked();

        if(action == MotionEvent.ACTION_DOWN) {
            JNIWrapper.touch_down(pointerId, event.getX(), event.getY());
        } else if((action & MotionEvent.ACTION_POINTER_DOWN) == MotionEvent.ACTION_POINTER_DOWN) {
            JNIWrapper.touch_down(pointerId, event.getX(index), event.getY(index));
        } else if(action == MotionEvent.ACTION_UP) {
            JNIWrapper.touch_up(pointerId, event.getX(), event.getY());
        } else if((action & MotionEvent.ACTION_POINTER_UP) == MotionEvent.ACTION_POINTER_UP) {
            JNIWrapper.touch_up(pointerId, event.getX(index), event.getY(index));
        } else if(action == MotionEvent.ACTION_MOVE) {
            for (int ii=0; ii<event.getPointerCount(); ++ii) {
                pointerId = event.getPointerId(ii);
                JNIWrapper.touch_move(pointerId, event.getX(ii), event.getY(ii));
            }
        }

        return true;
    }

    /** Renderer interface
     */
    private static class Renderer implements GLSurfaceView.Renderer
    {
        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            JNIWrapper.init(1, 1);
        }
        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            JNIWrapper.resize(width, height);
        }
        public void onDrawFrame(GL10 gl)
        {
            JNIWrapper.frame();
        }
    }
}
