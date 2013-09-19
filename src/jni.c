#include <jni.h>
#include <android/asset_manager.h>
#include "graphics.h"

extern _asset_manager;

static Graphics* _graphics = NULL;

JNIEXPORT void JNICALL Java_com_intel_hellogl_JNIWrapper_init(JNIEnv * env, jobject obj, int width, int height)
{
    _graphics = create_graphics(width, height);
}
JNIEXPORT void JNICALL Java_com_intel_hellogl_JNIWrapper_init_1asset_1manager(JNIEnv * env, jobject obj, jobject assetManager)
{
    _asset_manager = AAssetManager_fromJava(env, assetManager);
}
JNIEXPORT void JNICALL Java_com_intel_hellogl_JNIWrapper_frame(JNIEnv * env, jobject obj)
{
    render(_graphics);
}
