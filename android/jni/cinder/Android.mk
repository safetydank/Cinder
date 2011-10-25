#  Cinder on Android makefile
#

LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/Configure.mk

ifdef USE_FREEIMAGE
include $(CLEAR_VARS)

LOCAL_MODULE 	 := freeimage

FREEIMAGE_PATH    = $(LOCAL_PATH)/../FreeImage
LOCAL_C_INCLUDES := $(FREEIMAGE_PATH)/Source \
					$(FREEIMAGE_PATH)/Source/Metadata \
					$(FREEIMAGE_PATH)/Source/FreeImageToolkit \
					$(FREEIMAGE_PATH)/Source/LibJPEG \
					$(FREEIMAGE_PATH)/Source/LibMNG \
					$(FREEIMAGE_PATH)/Source/LibPNG \
					$(FREEIMAGE_PATH)/Source/LibTIFF \
					$(FREEIMAGE_PATH)/Source/ZLib \
					$(FREEIMAGE_PATH)/Source/LibOpenJPEG \
					$(FREEIMAGE_PATH)/Source/OpenEXR \
					$(FREEIMAGE_PATH)/Source/OpenEXR/Half \
					$(FREEIMAGE_PATH)/Source/OpenEXR/Iex \
					$(FREEIMAGE_PATH)/Source/OpenEXR/IlmImf \
					$(FREEIMAGE_PATH)/Source/OpenEXR/IlmThread \
					$(FREEIMAGE_PATH)/Source/OpenEXR/Imath \
					$(FREEIMAGE_PATH)/Source/LibRawLite \
					$(FREEIMAGE_PATH)/Source/LibRawLite/dcraw \
					$(FREEIMAGE_PATH)/Source/LibRawLite/internal \
					$(FREEIMAGE_PATH)/Source/LibRawLite/libraw \
					$(FREEIMAGE_PATH)/Source/LibRawLite/src

FREEIMAGE_SRC    = ../FreeImage/Source
LOCAL_SRC_FILES := $(FREEIMAGE_SRC)/FreeImage/BitmapAccess.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ColorLookup.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/FreeImage.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/FreeImageC.c \
				   $(FREEIMAGE_SRC)/FreeImage/FreeImageIO.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/GetType.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/MemoryIO.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PixelAccess.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/J2KHelper.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Plugin.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginBMP.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginCUT.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginDDS.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginEXR.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginG3.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginGIF.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginHDR.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginICO.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginIFF.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginJ2K.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginJP2.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginJPEG.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginKOALA.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginMNG.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPCD.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPCX.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPFM.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPICT.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPNG.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPNM.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginPSD.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginRAS.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginRAW.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginSGI.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginTARGA.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginTIFF.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginWBMP.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginXBM.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PluginXPM.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/PSDParser.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/TIFFLogLuv.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion16_555.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion16_565.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion24.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion32.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion4.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Conversion8.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ConversionFloat.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ConversionRGBF.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ConversionType.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ConversionUINT16.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/Halftoning.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/tmoColorConvert.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/tmoDrago03.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/tmoFattal02.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/tmoReinhard05.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ToneMapping.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/NNQuantizer.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/WuQuantizer.cpp \
				   $(FREEIMAGE_SRC)/DeprecationManager/Deprecated.cpp \
				   $(FREEIMAGE_SRC)/DeprecationManager/DeprecationMgr.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/CacheFile.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/MultiPage.cpp \
				   $(FREEIMAGE_SRC)/FreeImage/ZLibInterface.cpp \
				   $(FREEIMAGE_SRC)/Metadata/Exif.cpp \
				   $(FREEIMAGE_SRC)/Metadata/FIRational.cpp \
				   $(FREEIMAGE_SRC)/Metadata/FreeImageTag.cpp \
				   $(FREEIMAGE_SRC)/Metadata/IPTC.cpp \
				   $(FREEIMAGE_SRC)/Metadata/TagConversion.cpp \
				   $(FREEIMAGE_SRC)/Metadata/TagLib.cpp \
				   $(FREEIMAGE_SRC)/Metadata/XTIFF.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Background.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/BSplineRotate.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Channels.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/ClassicRotate.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Colors.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/CopyPaste.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Display.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Flip.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/JPEGTransform.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/MultigridPoissonSolver.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Rescale.cpp \
				   $(FREEIMAGE_SRC)/FreeImageToolkit/Resize.cpp \
				   $(FREEIMAGE_SRC)/LibJPEG/jaricom.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcapimin.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcapistd.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcarith.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jccoefct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jccolor.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcdctmgr.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jchuff.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcinit.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcmainct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcmarker.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcmaster.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcomapi.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcparam.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcprepct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jcsample.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jctrans.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdapimin.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdapistd.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdarith.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdatadst.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdatasrc.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdcoefct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdcolor.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jddctmgr.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdhuff.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdinput.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdmainct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdmarker.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdmaster.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdmerge.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdpostct.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdsample.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jdtrans.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jerror.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jfdctflt.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jfdctfst.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jfdctint.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jidctflt.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jidctfst.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jidctint.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jmemmgr.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jmemnobs.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jquant1.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jquant2.c \
				   $(FREEIMAGE_SRC)/LibJPEG/jutils.c \
				   $(FREEIMAGE_SRC)/LibJPEG/transupp.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_callback_xs.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_chunk_descr.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_chunk_io.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_chunk_prc.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_chunk_xs.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_cms.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_display.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_dither.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_error.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_filter.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_hlapi.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_jpeg.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_object_prc.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_pixels.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_prop_xs.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_read.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_trace.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_write.c \
				   $(FREEIMAGE_SRC)/LibMNG/libmng_zlib.c \
				   $(FREEIMAGE_SRC)/LibPNG/png.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngerror.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngget.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngmem.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngpread.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngread.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngrio.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngrtran.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngrutil.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngset.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngtrans.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngwio.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngwrite.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngwtran.c \
				   $(FREEIMAGE_SRC)/LibPNG/pngwutil.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_aux.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_close.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_codec.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_color.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_compress.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_dir.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_dirinfo.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_dirread.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_dirwrite.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_dumpmode.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_error.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_extension.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_fax3.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_fax3sm.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_flush.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_getimage.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_jpeg.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_luv.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_lzw.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_next.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_ojpeg.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_open.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_packbits.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_pixarlog.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_predict.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_print.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_read.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_strip.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_swab.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_thunder.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_tile.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_version.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_warning.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_write.c \
				   $(FREEIMAGE_SRC)/LibTIFF/tif_zip.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/bio.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/cio.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/dwt.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/event.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/image.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/j2k.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/j2k_lib.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/jp2.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/jpt.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/mct.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/mqc.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/openjpeg.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/pi.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/raw.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/t1.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/t2.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/tcd.c \
				   $(FREEIMAGE_SRC)/LibOpenJPEG/tgt.c \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfB44Compressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfBoxAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfChannelList.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfChannelListAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfChromaticities.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfChromaticitiesAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfCompressionAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfCompressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfConvert.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfCRgbaFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfDoubleAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfEnvmap.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfEnvmapAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfFloatAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfFrameBuffer.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfFramesPerSecond.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfHeader.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfHuf.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfInputFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfIntAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfIO.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfKeyCode.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfKeyCodeAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfLineOrderAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfLut.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfMatrixAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfMisc.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfOpaqueAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfOutputFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfPizCompressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfPreviewImage.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfPreviewImageAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfPxr24Compressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfRational.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfRationalAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfRgbaFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfRgbaYca.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfRleCompressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfScanLineInputFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfStandardAttributes.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfStdIO.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfStringAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfStringVectorAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTestFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfThreading.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTileDescriptionAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTiledInputFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTiledMisc.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTiledOutputFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTiledRgbaFile.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTileOffsets.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTimeCode.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfTimeCodeAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfVecAttribute.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfVersion.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfWav.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmImf/ImfZipCompressor.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathBox.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathColorAlgo.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathFun.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathMatrixAlgo.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathRandom.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathShear.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Imath/ImathVec.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Iex/IexBaseExc.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Iex/IexThrowErrnoExc.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/Half/half.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmThread/IlmThread.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmThread/IlmThreadMutex.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmThread/IlmThreadPool.cpp \
				   $(FREEIMAGE_SRC)/OpenEXR/IlmThread/IlmThreadSemaphore.cpp \
				   $(FREEIMAGE_SRC)/LibRawLite/internal/dcraw_common.cpp \
				   $(FREEIMAGE_SRC)/LibRawLite/internal/dcraw_fileio.cpp \
				   $(FREEIMAGE_SRC)/LibRawLite/internal/demosaic_packs.cpp \
				   $(FREEIMAGE_SRC)/LibRawLite/src/libraw_c_api.cpp \
				   $(FREEIMAGE_SRC)/LibRawLite/src/libraw_cxx.cpp 

include $(BUILD_STATIC_LIBRARY)

endif # USE_FREEIMAGE

ifdef USE_FREETYPE

include $(CLEAR_VARS)

FREETYPE_PATH = $(LOCAL_PATH)/../freetype-2.4.5

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
LOCAL_ARM_MODE := arm

FREETYPE_SRC = ../freetype-2.4.5/src

LOCAL_SRC_FILES := \
	$(FREETYPE_SRC)/base/ftbbox.c \
	$(FREETYPE_SRC)/base/ftbitmap.c \
	$(FREETYPE_SRC)/base/ftglyph.c \
	$(FREETYPE_SRC)/base/ftstroke.c \
	$(FREETYPE_SRC)/base/ftxf86.c \
	$(FREETYPE_SRC)/base/ftbase.c \
	$(FREETYPE_SRC)/base/ftsystem.c \
	$(FREETYPE_SRC)/base/ftinit.c \
	$(FREETYPE_SRC)/base/ftgasp.c \
	$(FREETYPE_SRC)/raster/raster.c \
	$(FREETYPE_SRC)/sfnt/sfnt.c \
	$(FREETYPE_SRC)/smooth/smooth.c \
	$(FREETYPE_SRC)/autofit/autofit.c \
	$(FREETYPE_SRC)/truetype/truetype.c \
	$(FREETYPE_SRC)/cff/cff.c \
	$(FREETYPE_SRC)/psnames/psnames.c \
	$(FREETYPE_SRC)/pshinter/pshinter.c

LOCAL_C_INCLUDES += \
	$(FREETYPE_PATH)/builds \
	$(FREETYPE_PATH)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_CFLAGS += -O2

LOCAL_MODULE:= ft2

include $(BUILD_STATIC_LIBRARY)

endif  # USE_FREETYPE

include $(CLEAR_VARS)

CINDER_SRC   = ../../../src/cinder
TESS_SRC     = ../../../src/libtess2
STBIMAGE_SRC = ../../../src/stb_image
UTF8_CPP_SRC = ../../../src/utf8-cpp/source

LOCAL_MODULE 	 := cinder

# required for wchar_t support, used by boost::filesystem
LOCAL_CFLAGS     := -D_GLIBCPP_USE_WCHAR_T -D__LITTLE_ENDIAN__

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include \
					$(LOCAL_PATH)/../../../boost \
					$(LOCAL_PATH)/$(TESS_SRC) \
					$(LOCAL_PATH)/$(UTF8_CPP_SRC) \
					$(LOCAL_PATH)/../freetype-2.4.5/include

LOCAL_SRC_FILES  := $(CINDER_SRC)/app/App.cpp \
					$(CINDER_SRC)/app/AppAndroid.cpp \
					$(CINDER_SRC)/app/Renderer.cpp \
					$(CINDER_SRC)/app/AppImplAndroidRendererGl.cpp \
					$(CINDER_SRC)/gl/gl.cpp \
					$(CINDER_SRC)/gl/Fbo.cpp \
					$(CINDER_SRC)/gl/Texture.cpp \
					$(CINDER_SRC)/gl/TextureFont.cpp \
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
					$(CINDER_SRC)/Area.cpp \
					$(CINDER_SRC)/AxisAlignedBox.cpp \
					$(CINDER_SRC)/BandedMatrix.cpp \
					$(CINDER_SRC)/BSpline.cpp \
					$(CINDER_SRC)/BSplineFit.cpp \
					$(CINDER_SRC)/Buffer.cpp \
					$(CINDER_SRC)/Camera.cpp \
					$(CINDER_SRC)/Channel.cpp \
					$(CINDER_SRC)/CinderMath.cpp \
					$(CINDER_SRC)/Color.cpp \
					$(CINDER_SRC)/DataSource.cpp \
					$(CINDER_SRC)/DataTarget.cpp \
					$(CINDER_SRC)/Exception.cpp \
					$(CINDER_SRC)/Font.cpp \
					$(CINDER_SRC)/ImageIo.cpp \
					$(CINDER_SRC)/Matrix.cpp \
					$(CINDER_SRC)/Path2d.cpp \
					$(CINDER_SRC)/Perlin.cpp \
					$(CINDER_SRC)/PolyLine.cpp \
					$(CINDER_SRC)/Rand.cpp \
					$(CINDER_SRC)/Ray.cpp \
					$(CINDER_SRC)/Rect.cpp \
					$(CINDER_SRC)/Shape2d.cpp \
					$(CINDER_SRC)/Sphere.cpp \
					$(CINDER_SRC)/Stream.cpp \
					$(CINDER_SRC)/Surface.cpp \
					$(CINDER_SRC)/SkylinePack.cpp \
					$(CINDER_SRC)/System.cpp \
					$(CINDER_SRC)/Text.cpp \
					$(CINDER_SRC)/Timer.cpp \
					$(CINDER_SRC)/Timeline.cpp \
					$(CINDER_SRC)/TimelineItem.cpp \
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
					$(TESS_SRC)/tess.c

ifdef USE_GLES2
LOCAL_SRC_FILES += $(CINDER_SRC)/gl/GlslProg.cpp \
					$(CINDER_SRC)/gl/Vbo.cpp

#					$(CINDER_SRC)/gl/gles2.cpp
#
LOCAL_CFLAGS += -DCINDER_GLES2
GLES_LDLIB = -lGLESv2
else
LOCAL_SRC_FILES += $(CINDER_SRC)/gl/Light.cpp \
					$(CINDER_SRC)/gl/Material.cpp \
					$(CINDER_SRC)/gl/TileRender.cpp
LOCAL_CFLAGS += -DCINDER_GLES1
GLES_LDLIB = -lGLESv1_CM
endif

ifdef USE_FREEIMAGE
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../FreeImage/Source
LOCAL_SRC_FILES  += $(CINDER_SRC)/ImageSourceFileFreeImage.cpp
LOCAL_CFLAGS     += -DCINDER_FREEIMAGE
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

LOCAL_STATIC_LIBRARIES	:= android_native_app_glue 

# Module exports
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../include $(LOCAL_PATH)/../../../boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL $(GLES_LDLIB) -lz

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)
