Cinder on Android
=================

This is a work-in-progress port of Cinder to Android.

When linked as an activity it uses the NDK Native Activity code, which
restricts it to running on Android versions 2.3 (Gingerbread) and upwards. This
restriction does not apply when linked as a static library called through JNI.

Tested to build on Windows (cygwin), OS X and Linux.


Quickstart
----------

* Install prerequisites:

  - Android SDK and Android NDK R5c or later
  - Boost, installed in the top Cinder directory
  - bash, curl, unzip, tar and bzip2 command-line tools (required by setup-android)

* Run setup-android

```
    % cd cinder/android
    % . ./setup-android
```

* Configure build settings by editing jni/cinder/Configure.mk (optional)
  
    USE_FREEIMAGE - use FreeImage image library (wide format compatibility but large)

    USE_STBIMAGE  - use the stb_image image library (small JPEG/PNG/GIF reader)

    USE_GLES2     - select OpenGL ES2 - if disabled then OpenGL ES1.5 is used instead

* Build the library.  Tip: use -j <num cores> to enable parallel build.

```
    % ndk-build -j 4
```

* Build one of the sample programs (AndroidTest, EaseGallery, FBOBasic,
  iPhoneAccelerometer, MultiTouchBasic, AndroidTestES2, TextureFont or
  shaderTestES2)

```
    % cd cinder/samples/<SAMPLE>/android
    % . ./setup-android
    % ndk-build && ant debug && adb install -r bin/<SAMPLE-APK>
```

  NB: The ES2 samples will only build against an ES2 build of Cinder


Linking against Cinder
----------------------

The android/setup-android script will suggest settings for linking against
prebuilt static libraries, or you can set NDK_MODULE_PATH to
$CINDER_PATH/android/jni to build a new copy of Cinder against each importing
project.

Each sample has a setup-android script fragment that will automatically set
NDK_MODULE_PATH if it hasn't already been defined.

See $NDK/docs/IMPORT-MODULE.html for more details of how to use
NDK_MODULE_PATH.


Orientation
-----------

To handle orientation changes, add "orientation" to configChanges in 
AndroidManifest.xml.  A change in orientation will trigger onResize() with
the new viewport dimensions.  The actual orientation can be checked inside
orientation by calling app->getOrientation().

To use fixed orientation, remove "orientation" from configChanges and add
android:orientation="landscape" to the activity tag.

Note that a fixed orientation app may be paused/restarted multiple times on
resumption.  It is important that you handle resume() correctly and quickly.
It is advisable to defer any heavy resource loading until the app starts its
update/draw cycle.


Activity lifecycle
------------------

OnStart  -> AppAndroid::setup()
OnResume -> AppAndroid::resume(bool renewContext)

If renewContext is true then the GL context has been renewed and all GL
resources (textures, shaders) will need to be recreated.

The default implementation of resume() just calls setup() if renewContext
is true.  This is unsuitable for non-trivial apps.

TODO: saved state onPause / onResume.

Status
------

* console() output goes to the Android debug log
* loadResource opens files embedded from the project "assets" folder
* FreeImage image source, tested to work with PNG/JPEG/BMP files so far
* stb_image image source, a smaller image backend compatible with PNG/JPEG/GIF
* OpenGL ES2 supported at compile time with a subset of the gl namespace
* OpenGL ES2 context class, supporting most gl namespace methods
* TextureFont implementation using Freetype


TODO
----

* Android save/restore state callbacks
* Audio support (using OpenSL ES)
* URL implementation (currently stubbed out)
* Camera capture (may require JNI)
* boost::filesystem v3 support


Credits
-------

Portions of this software are copyright © 2011 The FreeType Project (www.freetype.org).  All rights reserved.

stb_image (nothings.org) - public domain image library by Sean Barrett et al

utf8-cpp (utfcpp.sourceforge.net) - Copyright 2006 Nemanja Trifunovic

This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details.
FreeImage is used under the FIPL, version 1.0.

Parts of the font rendering code are based on freetype-gl (http://code.google.com/p/freetype-gl/)
Copyright 2011 Nicolas P. Rougier. All rights reserved


safetydank 23/8/11

