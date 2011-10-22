LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := AndroidTest
LOCAL_SRC_FILES := ../../src/AndroidTest.cpp

LOCAL_CFLAGS    := -DCINDER_GLES2
LOCAL_LDLIBS    := -landroid 
LOCAL_STATIC_LIBRARIES := cinder freeimage ft2 android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)

