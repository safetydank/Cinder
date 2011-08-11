LOCAL_PATH := $(call my-dir)

CINDER_PATH        := $(LOCAL_PATH)/../../../..
CINDER_MODULE_PATH := ../../../../android/obj/local/$(TARGET_ARCH_ABI)

include $(CINDER_PATH)/android/jni/CinderModules.mk

include $(CLEAR_VARS)

LOCAL_MODULE     := AndroidTestES2
LOCAL_C_INCLUDES := $(CINDER_PATH)/include \
					$(CINDER_PATH)/boost

LOCAL_SRC_FILES := ../../src/AndroidTestES2.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -lz
LOCAL_STATIC_LIBRARIES := cinder freeimage ft2 android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

