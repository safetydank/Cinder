
#include "cinder/CaptureImplAndroid.h"
#include "android/log.h"

#include <set>
using namespace std;

#if !defined(LOGD) && !defined(LOGI) && !defined(LOGE)
#define LOG_TAG "cinder"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

namespace cinder {

class AndroidCameraActivity;
class AndroidFrameGrabber;

class AndroidCameraActivity : public CameraActivity
{
public:
    AndroidCameraActivity(CaptureImplAndroid* capture)
    {
        mCapture = capture;
        mFramesReceived = 0;
    }

    virtual bool onFrameBuffer(void* buffer, int bufferSize)
    {
        if(isConnected() && buffer != 0 && bufferSize > 0)
        {
            mFramesReceived++;
            if (mCapture->mWaitingNextFrame || mCapture->mShouldAutoGrab)
            {
                pthread_mutex_lock(&mCapture->mNextFrameMutex);

                mCapture->setFrame(buffer, bufferSize);

                //pthread_cond_broadcast(&mCapture->mNextFrameCond); // to grab in sync
                pthread_mutex_unlock(&mCapture->mNextFrameMutex);
            }
            return true;
        }
        return false;
    }

    void LogFramesRate()
    {
        LOGI("FRAMES received: %d  grabbed: %d", mFramesReceived, mCapture->mFramesGrabbed);
    }

private:
    CaptureImplAndroid* mCapture;
    int mFramesReceived;
};

class AndroidFrameGrabber
{
public:
    AndroidFrameGrabber(CaptureImplAndroid* capture)
    {
        mCapture = capture;
        hasNewFrame = false;
    }

    bool hasNewFrame;

    bool grabFrame()
    {
        if( !mCapture->isOpened() ) {
            return false;
        }

        bool res = false;
        pthread_mutex_lock(&mCapture->mNextFrameMutex);
        if (mCapture->mCameraParamsChanged)
        {
            mCapture->mActivity->applyProperties();
            mCapture->mCameraParamsChanged = false;
            mCapture->mDataState = CaptureImplAndroid::CVCAPTURE_ANDROID_STATE_NO_FRAME; //we will wait new frame
        }

        if (mCapture->mDataState != CaptureImplAndroid::CVCAPTURE_ANDROID_STATE_HAS_NEW_FRAME_UNGRABBED) 
        {
            mCapture->mWaitingNextFrame = true;
            // grab frames in sync
            //pthread_cond_wait(&mCapture->mNextFrameCond, &mCapture->mNextFrameMutex);
        }

        if (mCapture->mDataState == CaptureImplAndroid::CVCAPTURE_ANDROID_STATE_HAS_NEW_FRAME_UNGRABBED) 
        {
            //swap current and new frames
            uint8_t* tmp = mCapture->mFrameYUV420;
            mCapture->mFrameYUV420 = mCapture->mFrameYUV420next;
            mCapture->mFrameYUV420next = tmp;

            mCapture->mDataState = CaptureImplAndroid::CVCAPTURE_ANDROID_STATE_HAS_FRAME_GRABBED;
            mCapture->mFramesGrabbed++;

            hasNewFrame = true;

            res=true;
        } 
        /*else 
        {
            LOGE("CaptureAndroid::checkNewFrame: NO new frame");
        }*/


        int res_unlock = pthread_mutex_unlock(&mCapture->mNextFrameMutex);

        if (res_unlock) 
        {
            LOGE("Error in CaptureAndroid::checkNewFrame: pthread_mutex_unlock returned %d --- probably, this object has been destroyed", res_unlock);
            return false;
        }

        return res;
    }

private:
    CaptureImplAndroid* mCapture;
};

bool CaptureImplAndroid::sDevicesEnumerated = false;
vector<Capture::DeviceRef> CaptureImplAndroid::sDevices;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CaptureImplAndroid

bool CaptureImplAndroid::Device::checkAvailable() const
{
    return true;
}

bool CaptureImplAndroid::Device::isConnected() const
{
    return true;
}

const vector<Capture::DeviceRef>& CaptureImplAndroid::getDevices( bool forceRefresh )
{
    if( sDevicesEnumerated && ( ! forceRefresh ) )
        return sDevices;

    sDevices.clear();

    // TODO check if we have 2 or 1 cameras
    sDevices.push_back( Capture::DeviceRef( new CaptureImplAndroid::Device( std::string((const char*)"Back Camera"), 0 ) ) );
    sDevices.push_back( Capture::DeviceRef( new CaptureImplAndroid::Device( std::string((const char*)"Front Camera"), 1 ) ) );

    sDevicesEnumerated = true;
    return sDevices;
}

CaptureImplAndroid::CaptureImplAndroid( int32_t width, int32_t height, const Capture::DeviceRef device )
{
    LOGI("CaptureImplAndroid: %dx%d id(%d)", width, height, device->getUniqueId());
    mDevice = device;
    open(device->getUniqueId());
    if(mIsCapturing)
    {
        // result in signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr deadbaad
        // looks like lib cant reinit internaly probaly only on selected phones
        //setFrameSize(width, height);

        if (mFrameFormat == ip::YUV_Unknown)
        {
            union {double prop; const char* name;} u;
            u.prop = (double)mActivity->getProperty(ANDROID_CAMERA_PROPERTY_PREVIEW_FORMAT_STRING);

            if (0 == strcmp(u.name, "yuv420sp"))
                mFrameFormat = ip::YUV_NV21;
            else if (0 == strcmp(u.name, "yvu420sp"))
                mFrameFormat = ip::YUV_NV12;
            else
                mFrameFormat = ip::YUV_Unknown;
        }
    }
}

void CaptureImplAndroid::open(int cameraId)
{
    //defaults
    mWidth               = 0;
    mHeight              = 0;
    mActivity            = 0;
    mIsOpened            = false;
    mFrameYUV420         = 0;
    mFrameYUV420next     = 0;
    mHasGray             = false;
    mHasColor            = false;
    mDataState           = CVCAPTURE_ANDROID_STATE_NO_FRAME;
    mWaitingNextFrame    = false;
    mShouldAutoGrab      = false;
    mFramesGrabbed       = 0;
    mCameraParamsChanged = false;
    mFrameFormat         = ip::YUV_Unknown;
    mIsCapturing         = false;
    mGotNewFrame         = false;
    mFrameGrabber        = 0;

    //try connect to camera
    mActivity = new AndroidCameraActivity(this);

    if (mActivity == 0) return;

    pthread_mutex_init(&mNextFrameMutex, NULL);
    //pthread_cond_init(&mNextFrameCond,  NULL);

    CameraActivity::ErrorCode errcode = mActivity->connect(cameraId);

    if(errcode == CameraActivity::NO_ERROR)
    {
        mIsOpened = true;
        mIsCapturing = true;
        mFrameGrabber = new AndroidFrameGrabber(this);
    }
    else
    {
        LOGE("Native_camera returned opening error: %d", errcode);
        delete mActivity;
        mActivity = 0;
    }
}

CaptureImplAndroid::~CaptureImplAndroid()
{
    if (mActivity)
    {
        ((AndroidCameraActivity*)mActivity)->LogFramesRate();


        pthread_mutex_lock(&mNextFrameMutex);

        uint8_t *tmp1=mFrameYUV420;
        uint8_t *tmp2=mFrameYUV420next;
        mFrameYUV420 = 0;
        mFrameYUV420next = 0;
        delete tmp1;
        delete tmp2;

        mCurrentFrame.reset();

        mDataState = CVCAPTURE_ANDROID_STATE_NO_FRAME;
        //pthread_cond_broadcast(&mNextFrameCond);

        pthread_mutex_unlock(&mNextFrameMutex);

        //mActivity->disconnect() will be automatically called inside destructor;
        delete mActivity;
        mActivity = 0;

        delete mFrameGrabber;
        mFrameGrabber = 0;

        pthread_mutex_destroy(&mNextFrameMutex);
        //pthread_cond_destroy(&mNextFrameCond);
    }
}

// needed to parse android properties
std::vector<std::string> &CaptureImplAndroid::splitString(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> CaptureImplAndroid::splitString(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    return CaptureImplAndroid::splitString(s, delim, elems);
}

void CaptureImplAndroid::setFrameSize(int width, int height)
{
    union {double prop; const char* name;} u;
    u.prop = mActivity->getProperty(ANDROID_CAMERA_PROPERTY_SUPPORTED_PREVIEW_SIZES_STRING);

    int best_w = width;
    int best_h = height;
    double minDiff = 10000000.0;
    double minDiff_w = 10000000.0;
    
    std::vector<std::string> sizes_str = splitString(u.name, ',');
    for(std::vector<std::string>::iterator it = sizes_str.begin(); it != sizes_str.end(); ++it) 
    {
        std::vector<std::string> wh_str = splitString(*it, 'x');
        double _w = ::atof(wh_str[0].c_str());
        double _h = ::atof(wh_str[1].c_str());
        if (std::abs(_h - height) < minDiff || (std::abs(_h - height) == minDiff && std::abs(_w - width) < minDiff_w)) 
        {
            best_w = (int)_w;
            best_h = (int)_h;
            minDiff = std::abs(_h - height);
            minDiff_w = std::abs(_w - width);
        }
    }

    mActivity->setProperty(ANDROID_CAMERA_PROPERTY_FRAMEWIDTH, best_w);
    mActivity->setProperty(ANDROID_CAMERA_PROPERTY_FRAMEHEIGHT, best_h);
    //
    //mActivity->applyProperties();
    mCameraParamsChanged = true;
    //mDataState = CVCAPTURE_ANDROID_STATE_NO_FRAME; //we will wait new frame
}

bool CaptureImplAndroid::checkNewFrame() const
{
    //return mDataState == CVCAPTURE_ANDROID_STATE_HAS_NEW_FRAME_UNGRABBED;
    return mFrameGrabber->grabFrame();
}

//Attention: this method should be called inside pthread_mutex_lock(mNextFrameMutex) only
void CaptureImplAndroid::setFrame(const void* buffer, int bufferSize)
{
    int _mWidth = mActivity->getFrameWidth();
    int _mHeight = mActivity->getFrameHeight();

    //allocate memory if needed
    prepareCacheForYUV(_mWidth, _mHeight);

    //copy data
    memcpy(mFrameYUV420next, buffer, bufferSize);

    mDataState = CVCAPTURE_ANDROID_STATE_HAS_NEW_FRAME_UNGRABBED;
    mWaitingNextFrame = false;//set flag that no more frames required at this moment
}

void CaptureImplAndroid::prepareCacheForYUV(int width, int height)
{
    if (mWidth!=width || mHeight!=height)
    {
        LOGD("prepareCacheForYUV: Changing size of buffers: from width=%d height=%d to width=%d height=%d", mWidth, mHeight, width, height);
        
        uint8_t *tmp = mFrameYUV420next;
        mFrameYUV420next = new uint8_t [width * height * 3 / 2];
        if (tmp != NULL) {
            delete[] tmp;
        }

        tmp = mFrameYUV420;
        mFrameYUV420 = new uint8_t [width * height * 3 / 2];
        if (tmp != NULL) {
            delete[] tmp;
        }

        mWidth = width;
        mHeight = height;

        mCurrentFrame.reset();
        mCurrentFrame = Surface8u( width, height, false, SurfaceChannelOrder::RGB );
    }
}

void CaptureImplAndroid::start()
{
    if( mIsCapturing ) return;

    mIsCapturing = true;
}

void CaptureImplAndroid::stop()
{
    if( ! mIsCapturing ) return;

    // TODO
    
    mIsCapturing = false;
}

bool CaptureImplAndroid::isCapturing()
{
    return mIsCapturing;
}

Surface8u CaptureImplAndroid::getSurface() const
{
    // Attention! 
    // all the operations in this function below should occupy less time 
    // than the period between two frames from camera
    uint8_t *current_frameYUV420 = mFrameYUV420;
    //if(mFrameGrabber->grabFrame())
    if(NULL != current_frameYUV420)
    {   
        if(mFrameGrabber->hasNewFrame)
        {
            ip::YUVConvert(current_frameYUV420, mFrameFormat, mWidth, mHeight*3/2, &mCurrentFrame);
            mFrameGrabber->hasNewFrame = false;
        }
    }
    
    return mCurrentFrame;
}

} // namespace

