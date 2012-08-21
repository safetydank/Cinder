LOCAL_PATH := $(call my-dir)/$(TARGET_ARCH_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r220
LOCAL_SRC_FILES := libnative_camera_r2.2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r233
LOCAL_SRC_FILES := libnative_camera_r2.3.3.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r301
LOCAL_SRC_FILES := libnative_camera_r3.0.1.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r400
LOCAL_SRC_FILES := libnative_camera_r4.0.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r403
LOCAL_SRC_FILES := libnative_camera_r4.0.3.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ocv_camera_r411
LOCAL_SRC_FILES := libnative_camera_r4.1.1.so
include $(PREBUILT_SHARED_LIBRARY)

