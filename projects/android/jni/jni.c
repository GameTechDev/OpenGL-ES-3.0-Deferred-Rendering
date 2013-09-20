#include <jni.h>
#include <android/asset_manager_jni.h>
#include "graphics.h"
#include "timer.h"
#include "system.h"

#define UNUSED_PARAMETER(param) (void)sizeof((param))

extern AAssetManager* _asset_manager;

static Graphics*    _graphics = NULL;
static Timer*       _timer = NULL;

JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_init(JNIEnv * env, jobject obj, int width, int height)
{
    _graphics = create_graphics(width, height);
    _timer = create_timer();

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_init_1asset_1manager(JNIEnv * env, jobject obj, jobject assetManager)
{
    _asset_manager = AAssetManager_fromJava(env, assetManager);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_frame(JNIEnv * env, jobject obj)
{
    static float time = 0.0f;
    float delta_time = (float)get_delta_time(_timer);
    //float delta_time = 1.0f/60.0f;
    time += delta_time;
    //system_log("%.3f\n", delta_time);
    //system_log("%.3f\n", get_running_time(_timer));
    if(time > 1.0f) {
        system_log("%f\n", get_running_time(_timer));
        time -= 1.0f;
    }
    //system_log("%llu\n", _timer.prev_time - _timer.start_time);
    render(_graphics);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
