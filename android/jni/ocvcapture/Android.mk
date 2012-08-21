#  OpenCV Native Camera makefile
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ocvcapture_activity
LOCAL_SRC_FILES := camera_activity.cpp
LOCAL_LDLIBS +=  -llog -ldl 
LOCAL_CPPFLAGS += -frtti -fexceptions -O2 
LOCAL_SHARED_LIBRARIES += liblog

include $(BUILD_STATIC_LIBRARY)
