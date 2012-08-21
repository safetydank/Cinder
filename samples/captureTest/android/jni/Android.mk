LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := CaptureTest

LOCAL_SRC_FILES := ../../src/captureTest.cpp

LOCAL_LDLIBS    := -landroid -ldl
LOCAL_STATIC_LIBRARIES := cinder boost_system boost_filesystem freeimage ft2 ocvcapture_activity android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)
$(call import-module,boost)
$(call import-module,ocvcapture)

