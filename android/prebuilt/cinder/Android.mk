LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/../../jni/cinder/Configure.mk

ifdef USE_FREEIMAGE
	include $(CLEAR_VARS)
	LOCAL_MODULE := freeimage
	LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libfreeimage.a
	include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := ft2
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cairo
LOCAL_SRC_FILES := ../../../lib/android/$(TARGET_ARCH_ABI)/libcairo.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pixman
LOCAL_SRC_FILES := ../../../lib/android/$(TARGET_ARCH_ABI)/libpixman.a
include $(PREBUILT_STATIC_LIBRARY)

ifdef USE_OCV_CAPTURE
	include $(CLEAR_VARS)
	LOCAL_MODULE := ocvcapture_activity
	LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libocvcapture_activity.a
	include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../jni/cinder/Configure.mk
LOCAL_MODULE := cinder
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libcinder.a
LOCAL_EXPORT_CFLAGS += -D_GLIBCPP_USE_WCHAR_T -D__LITTLE_ENDIAN__
LOCAL_EXPORT_CFLAGS += -Wno-psabi -Wno-overflow
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../include $(LOCAL_PATH)/../../../boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL -lz

ifdef USE_GLES2
	LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../../blocks/propipe/src
	LOCAL_EXPORT_CFLAGS += -DCINDER_GLES2
	LOCAL_EXPORT_LDLIBS += -lGLESv2
else
	LOCAL_EXPORT_CFLAGS += -DCINDER_GLES1
	LOCAL_EXPORT_LDLIBS += -lGLESv1_CM
endif

include $(PREBUILT_STATIC_LIBRARY)

