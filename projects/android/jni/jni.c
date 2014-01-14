#include <jni.h>
#include <sys/types.h>
#include <android/asset_manager_jni.h>
#include "game.h"
#include "system.h"

#define UNUSED_PARAMETER(param) (void)sizeof((param))

extern AAssetManager* _asset_manager;

static Game* _game = NULL;

JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_init(JNIEnv * env, jobject obj, int width, int height)
{
    _game = create_game();

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
    UNUSED_PARAMETER(width);
    UNUSED_PARAMETER(height);
}
JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_resize(JNIEnv * env, jobject obj, int width, int height)
{
    resize_game(_game, width, height);

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
    update_game(_game);
    render_game(_game);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_touch_1down(JNIEnv * env, jobject obj, int index, float x, float y)
{
    TouchPoint point = {
        index,
        { x, y }
    };
    add_touch_points(_game, 1, &point);
    //system_log("%d down: %f, %f\n", index, x, y);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_touch_1up(JNIEnv * env, jobject obj, int index, float x, float y)
{
    TouchPoint point = {
        index,
        { x, y }
    };
    remove_touch_points(_game, 1, &point);
    //system_log("%d up: %f, %f\n", index, x, y);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}

JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_touch_1move(JNIEnv * env, jobject obj, int index, float x, float y)
{
    TouchPoint point = {
        index,
        { x, y }
    };
    update_touch_points(_game, 1, &point);
    //system_log("%d move: %f, %f\n", index, x, y);
    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}

