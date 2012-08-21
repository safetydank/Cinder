#pragma once

#include <pthread.h>

#include "cinder/Cinder.h"
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "cinder/ip/YUVConvert.h"
#include "camera_activity.hpp"

namespace cinder {

class CaptureImplAndroid {
 public:
    class Device;

    CaptureImplAndroid( int32_t width, int32_t height, const Capture::DeviceRef device );
    CaptureImplAndroid( int32_t width, int32_t height );
    ~CaptureImplAndroid();
    
    void start();
    void stop();
    
    bool        isCapturing();
    bool        checkNewFrame() const;

    int32_t     getWidth() const { return mWidth; }
    int32_t     getHeight() const { return mHeight; }
    
    Surface8u   getSurface() const;
    
    const Capture::DeviceRef getDevice() const { return mDevice; }
    
    static const std::vector<Capture::DeviceRef>&   getDevices( bool forceRefresh = false );

    class Device : public Capture::Device {
      public:
        bool                        checkAvailable() const;
        bool                        isConnected() const;
        Capture::DeviceIdentifier   getUniqueId() const { return mUniqueId; }

        Device( const std::string &name, int uniqueId ) : Capture::Device(), mUniqueId( uniqueId ) { mName = name; }
     protected:
        int             mUniqueId;
    };
 protected:
    void    init( int32_t width, int32_t height, const Capture::Device &device );

    int                             mDeviceID;
    bool                            mIsCapturing;
    bool                            mGotNewFrame;

    int32_t             mWidth, mHeight;
    mutable Surface8u   mCurrentFrame;
    Capture::DeviceRef  mDevice;

    static bool                         sDevicesEnumerated;
    static std::vector<Capture::DeviceRef>  sDevices;

    // android specific
    CameraActivity* mActivity;
    class AndroidFrameGrabber* mFrameGrabber;

    uint8_t *mFrameYUV420;
    uint8_t *mFrameYUV420next;

    ip::YUVFormat mFrameFormat;

//private: 
    bool mIsOpened;
    bool mCameraParamsChanged;

    //frames counter for statistics
    int mFramesGrabbed;

    //cached converted frames
    bool mHasGray;
    bool mHasColor;

    enum Capture_Android_DataState 
    {
        CVCAPTURE_ANDROID_STATE_NO_FRAME=0,
        CVCAPTURE_ANDROID_STATE_HAS_NEW_FRAME_UNGRABBED,
        CVCAPTURE_ANDROID_STATE_HAS_FRAME_GRABBED
    };
    volatile Capture_Android_DataState mDataState;

    //synchronization
    pthread_mutex_t mNextFrameMutex;
    pthread_cond_t mNextFrameCond;
    volatile bool mWaitingNextFrame;
    volatile bool mShouldAutoGrab;

    void setFrameSize(int width, int height);
    void setFrame(const void* buffer, int bufferSize);
    bool grabFrame();
    void open(int);
    bool isOpened() const { return mIsOpened; }
    void traceDir(const std::string& folderPath);
    void prepareCacheForYUV(int width, int height);

    // some utils needed for Android Capture
    static std::vector<std::string> &splitString(const std::string &s, char delim, std::vector<std::string> &elems);
    static std::vector<std::string> splitString(const std::string &s, char delim);

    friend class AndroidCameraActivity;
    friend class AndroidFrameGrabber;
};

} //namespace

