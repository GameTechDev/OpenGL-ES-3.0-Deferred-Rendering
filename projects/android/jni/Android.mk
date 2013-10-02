LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

INCLUDES    +=  -I../../src -I../../external -I../../
DEFINES     +=

C_STD   = -std=gnu89
CXX_STD = -std=c++98
WARNINGS    +=

LOCAL_MODULE    := libandroidinterface
LOCAL_CFLAGS    := $(INCLUDES) $(WARNINGS) $(C_STD)
LOCAL_CXXFLAGS  := $(INCLUDES) $(WARNINGS) $(CXX_STD)
LOCAL_SRC_FILES := 	jni.c \
					../../../src/android/system_android.c \
					../../../src/graphics.c \
					../../../src/timer.c \
                    ../../../src/game.c \
                    ../../../src/mesh.c \
                    ../../../src/program.c \
                    ../../../src/forward.c \
                    ../../../src/light_prepass.c \
                    ../../../src/utility.c \
                    ../../../src/texture.c \
                    ../../../src/scene.cpp \
                    ../../../external/stb_image.c
LOCAL_LDLIBS := -lGLESv2 -lEGL -llog -landroid

include $(BUILD_SHARED_LIBRARY)