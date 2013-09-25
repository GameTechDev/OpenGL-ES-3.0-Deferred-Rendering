LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

INCLUDES    +=  -I../../src -I../../src/external
DEFINES     +=

C_STD   = -std=gnu89
CXX_STD = -std=c++98
WARNINGS    +=   -Wall -Wextra -Wpointer-arith \
                 -Wwrite-strings  -Wredundant-decls -Winline -Wno-long-long \
                 -Werror -Wuninitialized

LOCAL_MODULE    := libandroidinterface
LOCAL_CFLAGS    := $(INCLUDES) $(WARNINGS) $(C_STD)
LOCAL_SRC_FILES := 	jni.c \
					../../../src/android/system_android.c \
					../../../src/graphics.c \
					../../../src/timer.c \
                    ../../../src/game.c \
                    ../../../src/gl_helper.c \
                    ../../../src/external/stb_image.c
LOCAL_LDLIBS := -lGLESv2 -lEGL -llog -landroid

include $(BUILD_SHARED_LIBRARY)