CINDER_JNI_PATH = $(call my-dir)
include $(CINDER_JNI_PATH)/cinder/Configure.mk

APP_ABI := armeabi-v7a
APP_PLATFORM := android-9
APP_STL      := gnustl_static
APP_MODULES  := cinder
APP_CPPFLAGS := -fexceptions -frtti

ifdef USE_ARMEABI_V7A
APP_ABI += armeabi-v7a
endif

ifdef USE_FREEIMAGE
APP_MODULES += freeimage
endif

ifdef USE_FREETYPE
APP_MODULES += ft2
endif

ifdef USE_OCV_CAPTURE
APP_MODULES += ocvcapture_activity
endif

