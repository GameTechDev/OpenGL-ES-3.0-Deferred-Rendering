LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libandroidinterface
LOCAL_CFLAGS    := -I../../src
LOCAL_SRC_FILES := 	jni.c \
					../../../src/android/system_android.c \
					../../../src/graphics.c
LOCAL_LDLIBS := -lGLESv2 -lEGL -llog -landroid

include $(BUILD_SHARED_LIBRARY)