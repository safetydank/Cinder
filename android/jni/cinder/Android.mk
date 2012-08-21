#  Cinder on Android makefile
#

LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/Configure.mk

ifdef USE_FREEIMAGE
include $(TOP_PATH)/FreeImage/Android.mk
endif # USE_FREEIMAGE

ifdef USE_FREETYPE
include $(TOP_PATH)/freetype-2.4.5/Android.mk
endif  # USE_FREETYPE

ifdef USE_OCV_CAPTURE
include $(TOP_PATH)/ocvcapture/Android.mk
endif

include $(CLEAR_VARS)

CINDER_SRC   = ../../../src/cinder
TESS_SRC     = ../../../src/libtess2
STBIMAGE_SRC = ../../../src/stb_image
UTF8_CPP_SRC = ../../../include/utf8-cpp
JSONCPP_SRC  = ../../../src/jsoncpp

LOCAL_MODULE 	 := cinder

# required for wchar_t support, used by boost::filesystem
LOCAL_CFLAGS     := -D_GLIBCPP_USE_WCHAR_T -D__LITTLE_ENDIAN__

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include \
					$(LOCAL_PATH)/../../../boost \
					$(LOCAL_PATH)/$(TESS_SRC) \
					$(LOCAL_PATH)/$(UTF8_CPP_SRC) \
					$(LOCAL_PATH)/../freetype-2.4.5/include \
					$(LOCAL_PATH)/../../../include/android/cairo

LOCAL_SRC_FILES  := $(CINDER_SRC)/app/App.cpp \
					$(CINDER_SRC)/app/AppAndroid.cpp \
					$(CINDER_SRC)/app/Renderer.cpp \
					$(CINDER_SRC)/app/AppImplAndroidRendererGl.cpp \
					$(CINDER_SRC)/cairo/Cairo.cpp \
					$(CINDER_SRC)/gl/gl.cpp \
					$(CINDER_SRC)/gl/Fbo.cpp \
					$(CINDER_SRC)/gl/Texture.cpp \
					$(CINDER_SRC)/gl/TextureFont.cpp \
					$(CINDER_SRC)/gl/TextureFontAtlas.cpp \
					$(CINDER_SRC)/ip/Blend.cpp \
					$(CINDER_SRC)/ip/EdgeDetect.cpp \
					$(CINDER_SRC)/ip/Fill.cpp \
					$(CINDER_SRC)/ip/Flip.cpp \
					$(CINDER_SRC)/ip/Grayscale.cpp \
					$(CINDER_SRC)/ip/Hdr.cpp \
					$(CINDER_SRC)/ip/Premultiply.cpp \
					$(CINDER_SRC)/ip/Resize.cpp \
					$(CINDER_SRC)/ip/Threshold.cpp \
					$(CINDER_SRC)/ip/Trim.cpp \
					$(CINDER_SRC)/ip/YUVConvert.cpp \
					$(CINDER_SRC)/svg/Svg.cpp \
					$(CINDER_SRC)/Area.cpp \
					$(CINDER_SRC)/AxisAlignedBox.cpp \
					$(CINDER_SRC)/BandedMatrix.cpp \
					$(CINDER_SRC)/Base64.cpp \
					$(CINDER_SRC)/BinPack.cpp \
					$(CINDER_SRC)/BSpline.cpp \
					$(CINDER_SRC)/BSplineFit.cpp \
					$(CINDER_SRC)/Buffer.cpp \
					$(CINDER_SRC)/Camera.cpp \
					$(CINDER_SRC)/Capture.cpp \
					$(CINDER_SRC)/Channel.cpp \
					$(CINDER_SRC)/CinderMath.cpp \
					$(CINDER_SRC)/Color.cpp \
					$(CINDER_SRC)/DataSource.cpp \
					$(CINDER_SRC)/DataTarget.cpp \
					$(CINDER_SRC)/Exception.cpp \
					$(CINDER_SRC)/Font.cpp \
					$(CINDER_SRC)/Frustum.cpp \
					$(CINDER_SRC)/ImageIo.cpp \
					$(CINDER_SRC)/Json.cpp \
					$(CINDER_SRC)/Matrix.cpp \
					$(CINDER_SRC)/Path2d.cpp \
					$(CINDER_SRC)/Perlin.cpp \
					$(CINDER_SRC)/PolyLine.cpp \
					$(CINDER_SRC)/Plane.cpp \
					$(CINDER_SRC)/Rand.cpp \
					$(CINDER_SRC)/Ray.cpp \
					$(CINDER_SRC)/Rect.cpp \
					$(CINDER_SRC)/Shape2d.cpp \
					$(CINDER_SRC)/Sphere.cpp \
					$(CINDER_SRC)/Stream.cpp \
					$(CINDER_SRC)/Surface.cpp \
					$(CINDER_SRC)/System.cpp \
					$(CINDER_SRC)/Text.cpp \
					$(CINDER_SRC)/TextEngine.cpp \
					$(CINDER_SRC)/TextEngineFreeType.cpp \
					$(CINDER_SRC)/Timeline.cpp \
					$(CINDER_SRC)/TimelineItem.cpp \
					$(CINDER_SRC)/Timer.cpp \
					$(CINDER_SRC)/Triangulate.cpp \
					$(CINDER_SRC)/TriMesh.cpp \
					$(CINDER_SRC)/Tween.cpp \
					$(CINDER_SRC)/Url.cpp \
					$(CINDER_SRC)/UrlImplAndroid.cpp \
					$(CINDER_SRC)/Utilities.cpp \
					$(CINDER_SRC)/Xml.cpp \
					$(TESS_SRC)/bucketalloc.c \
					$(TESS_SRC)/dict.c \
					$(TESS_SRC)/geom.c \
					$(TESS_SRC)/mesh.c \
					$(TESS_SRC)/priorityq.c \
					$(TESS_SRC)/sweep.c \
					$(TESS_SRC)/tess.c \
					$(JSONCPP_SRC)/json_reader.cpp \
					$(JSONCPP_SRC)/json_value.cpp \
					$(JSONCPP_SRC)/json_writer.cpp

ifdef USE_GLES2

# Propipe block
PROPIPE_SRC  = ../../../blocks/propipe/src
# Built-in propipe block
LOCAL_SRC_FILES  += $(PROPIPE_SRC)/propipe/Context.cpp \
                    $(PROPIPE_SRC)/propipe/Draw.cpp \
                    $(PROPIPE_SRC)/propipe/DrawShader.cpp \
                    $(PROPIPE_SRC)/propipe/Matrices.cpp \
                    $(PROPIPE_SRC)/propipe/TextureFont.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PROPIPE_SRC)

LOCAL_SRC_FILES  += $(CINDER_SRC)/gl/GlslProg.cpp \
                    $(CINDER_SRC)/gl/Vbo.cpp
LOCAL_CFLAGS += -DCINDER_GLES2
GLES_LDLIB = -lGLESv2
else

LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(TWEAKBAR_SRC)
LOCAL_SRC_FILES += $(CINDER_SRC)/gl/Light.cpp \
                   $(CINDER_SRC)/gl/Material.cpp \
                   $(CINDER_SRC)/gl/TileRender.cpp

# TWEAKBAR_SRC   = ../../../src/AntTweakBar
# LOCAL_SRC_FILES += \
#				   $(CINDER_SRC)/params/Params.cpp \
#				   $(TWEAKBAR_SRC)/TwBar.cpp \
#				   $(TWEAKBAR_SRC)/TwColors.cpp \
#				   $(TWEAKBAR_SRC)/TwFonts.cpp \
#				   $(TWEAKBAR_SRC)/TwMgr.cpp \
#				   $(TWEAKBAR_SRC)/TwOpenGL.cpp

LOCAL_CFLAGS += -DCINDER_GLES1
GLES_LDLIB = -lGLESv1_CM
endif

ifdef USE_FREEIMAGE
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../FreeImage/Source
LOCAL_SRC_FILES  += $(CINDER_SRC)/ImageSourceFileFreeImage.cpp
LOCAL_CFLAGS     += -DCINDER_FREEIMAGE
endif

ifdef USE_OCV_CAPTURE
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../ocvcapture
LOCAL_SRC_FILES  += $(CINDER_SRC)/CaptureImplAndroid.cpp
LOCAL_CFLAGS     += -DCINDER_OCVCAPTURE
endif

ifdef USE_STBIMAGE
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/stb_image
LOCAL_SRC_FILES  += $(STBIMAGE_SRC)/stb_image.c \
                    $(CINDER_SRC)/ImageSourceFileStbImage.cpp
LOCAL_CFLAGS     += -DCINDER_STBIMAGE
endif

#  Android Asset Manager requires to be built against 2.3+
ifdef USE_ASSET_MANAGER
LOCAL_CFLAGS    += -DCINDER_AASSET
endif

LOCAL_CFLAGS += -Wno-psabi -Wno-overflow

LOCAL_STATIC_LIBRARIES	:= android_native_app_glue 

# Module exports
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../include $(LOCAL_PATH)/../../../boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL $(GLES_LDLIB) -lz -ldl

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)

