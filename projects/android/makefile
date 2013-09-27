# Makefile
#
# NOTES:
# 1) MACRO_NAME ?= "value" means "value" is default value if MACRO_NAME not already defined
# 2) You can override the value of MACRO_NAME on the command line by doing:
#    > make MACRO_NAME="command line value" <target>

# Default values
ANDROID_APP_NAME ?= "DeferredGLES"
ANDROID_APP_ACTIVITY ?= $(ANDROID_APP_NAME)Activity
ANDROID_APP_PATH ?= "."
ANDROID_TARGET_ID ?= "android-18"
ANDROID_PACKAGE_NAME ?= "com.intel.deferredgles"

TARGET_DEVICE:=

# > make
#
# 1) debug
all: debug

# > make project
#
# Create Android project with the values defined above.
# Also, create skeleton structure for native (JNI) code
project:
	android create project --name $(ANDROID_APP_NAME) \
												 --target $(ANDROID_TARGET_ID) \
												 --path $(ANDROID_APP_PATH) \
												 --activity $(ANDROID_APP_ACTIVITY) \
												 --package $(ANDROID_PACKAGE_NAME)
	# add the native directory and initial files
	mkdir jni
	touch jni/Android.mk
	touch jni/Application.mk
	touch jni/$(ANDROID_APP_NAME)JNI.c

# > make init
#
# Update an existing Android project with values defined above
init:
	android update project 	--target $(ANDROID_TARGET_ID) \
						 	--path $(ANDROID_APP_PATH) \

# > make clean
#
# Remove all the auto generated files
clean:
	ndk-build clean
	ant clean

# > make native
#
# Only build the native (C and/or C++) code
native:
	ndk-build -j

# > make debug
#
# Build the Java side and package everything into an APK.
# Will trigger "make native" to build the native code
debug: native
	ant debug

# > make install
#
# Deploy the APK to a device (or emulator)
# Will trigger "make debug" to build the APK
install: debug
	adb $(TARGET_DEVICE) install -r bin/DeferredGLES-debug.apk
	#ant installd

# > make run
#
# Run main activity of the installed APK
# Assuming the APK is installed...won't trigger a build/install
run: install
	adb $(TARGET_DEVICE) shell am start -n $(ANDROID_PACKAGE_NAME)/.$(ANDROID_APP_NAME)

# > make kill
#
# If the app is running, kills it
kill:
	adb $(TARGET_DEVICE) shell am force-stop $(ANDROID_PACKAGE_NAME) -s emulator-5554

GenerateHeaders:
	javah -d ../src/include -classpath ./bin/classes/ $(ANDROID_PACKAGE_NAME).NativeLib