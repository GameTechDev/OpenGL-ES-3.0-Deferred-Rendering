#include <jni.h>
#include <android/asset_manager_jni.h>
#include "game.h"

#define UNUSED_PARAMETER(param) (void)sizeof((param))

extern AAssetManager* _asset_manager;

static Game* _game = NULL;

JNIEXPORT void JNICALL Java_com_intel_deferredgles_JNIWrapper_init(JNIEnv * env, jobject obj, int width, int height)
{
    _game = create_game(width, height);

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
    game_update(_game);
    game_render(_game);

    UNUSED_PARAMETER(env);
    UNUSED_PARAMETER(obj);
}
