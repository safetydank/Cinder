#include "cinder/app/AppAndroid.h"
#include "cinder/Stream.h"

#include <errno.h>

#include <android/asset_manager.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <map>

using std::vector;
using std::map;
using ci::app::TouchEvent;
using ci::app::Orientation_t;

enum ActivityState {
    ACTIVITY_START = 0,
    ACTIVITY_RESUME,
    ACTIVITY_PAUSE,
    ACTIVITY_STOP,
    ACTIVITY_DESTROY
};

typedef map<int32_t, TouchEvent::Touch> ActiveTouchMap;
struct TouchState {
    vector<TouchEvent::Touch> touchesBegan;
    vector<TouchEvent::Touch> touchesMoved;
    vector<TouchEvent::Touch> touchesEnded;

    ActiveTouchMap activeTouches;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* androidApp;
    void* savedState;

    ASensorManager*    sensorManager;
    const ASensor*     accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;

    ci::app::AppAndroid* cinderApp;
    ci::app::Renderer*   cinderRenderer;

    TouchState* touchState;

    //  accelerometer
    bool  accelEnabled;
    float accelUpdateFrequency;
    ActivityState activityState;

    Orientation_t orientation;
    bool renewContext;
    bool setupCompleted;
    bool resumed;

    //  JNI access helpers
    JavaVM* vm;
};

static void updateWindowSize(struct engine* engine)
{
    if (engine->androidApp->window) {
        int32_t winWidth = ANativeWindow_getWidth(engine->androidApp->window);
        int32_t winHeight = ANativeWindow_getHeight(engine->androidApp->window);
        if ( winWidth != engine->cinderApp->mWidth || winHeight != engine->cinderApp->mHeight ) {
            engine->cinderApp->setWindowSize(winWidth, winHeight);
            engine->cinderApp->privateResize__(ci::Vec2i(winWidth, winHeight));
        }
    }
}
 
static void engine_draw_frame(struct engine* engine) {
    ci::app::AppAndroid& app      = *(engine->cinderApp);
    ci::app::Renderer&   renderer = *(engine->cinderRenderer);

    if (!renderer.isValidDisplay()) {
        CI_LOGW("XXX NO VALID DISPLAY, SKIPPING RENDER");
        // No display.
        return;
    }

    //  XXX handles delayed window size updates from orientation changes
    updateWindowSize(engine);

    // XXX startDraw not necessary?
    // renderer.startDraw();
    app.privateUpdate__();
    app.privateDraw__();
    renderer.finishDraw();
}




/**
 * Touch input handlers ///////////////////////////////////////////////////////////////////////////////////////
 * replaces engine_update_touches()
 */
static void engine_update_active_touches( ci::app::AppAndroid& app, TouchState* touchState )
{
    vector<ci::app::TouchEvent::Touch> activeList;
    
    ActiveTouchMap& activeMap = touchState->activeTouches;
    for (ActiveTouchMap::iterator it = activeMap.begin(); it != activeMap.end(); ++it) {
        activeList.push_back(it->second);
    }
    app.privateSetActiveTouches__(activeList);
}

static void engine_update_touches_began( ci::app::AppAndroid& app, TouchState* touchState )
{
    if ( app.getSettings().isMultiTouchEnabled() ){
        if ( ! touchState->touchesBegan.empty() ) {
            app.privateTouchesBegan__( ci::app::TouchEvent( touchState->touchesBegan ) );
            touchState->touchesBegan.clear();
        }
    } else {
        if ( ! touchState->touchesBegan.empty() ) {
            const float contentScale = 1.0f;
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesBegan.begin(); it != touchState->touchesBegan.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesBegan.clear();
        }
    }
}

static void engine_update_touches_moved( ci::app::AppAndroid& app, TouchState* touchState )
{
    if ( app.getSettings().isMultiTouchEnabled() ){
        if ( ! touchState->touchesMoved.empty() ) {
            app.privateTouchesMoved__( ci::app::TouchEvent( touchState->touchesMoved ) );
            touchState->touchesMoved.clear();
        }
    } else {
        if ( ! touchState->touchesMoved.empty() ) {
            const float contentScale = 1.0f;
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesMoved.begin(); it != touchState->touchesMoved.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesMoved.clear();
        }
    }
}

static void engine_update_touches_ended( ci::app::AppAndroid& app, TouchState* touchState )
{
    if ( app.getSettings().isMultiTouchEnabled() ){
        if ( ! touchState->touchesEnded.empty() ) {
            app.privateTouchesEnded__( ci::app::TouchEvent( touchState->touchesEnded ) );
            touchState->touchesEnded.clear();
        }
    } else {
        if ( ! touchState->touchesEnded.empty() ) {
            const float contentScale = 1.0f;
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesEnded.begin(); it != touchState->touchesEnded.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesEnded.clear();
        }
    }
}


/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    static const char* actionNames[] = {
        "AMOTION_EVENT_ACTION_DOWN",
        "AMOTION_EVENT_ACTION_UP",
        "AMOTION_EVENT_ACTION_MOVE",
        "AMOTION_EVENT_ACTION_CANCEL",
        "AMOTION_EVENT_ACTION_OUTSIDE",
        "AMOTION_EVENT_ACTION_POINTER_DOWN",
        "AMOTION_EVENT_ACTION_POINTER_UP",
    };
    
    struct engine* engine = (struct engine*)app->userData;
    struct ci::app::AppAndroid& cinderApp = *(engine->cinderApp);
    struct TouchState* touchState = engine->touchState;
    
    int32_t eventType = AInputEvent_getType(event);
    
    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
        int32_t actionCode = AMotionEvent_getAction(event);
        int action = actionCode & AMOTION_EVENT_ACTION_MASK;
        int index  = (actionCode & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        const char* actionName = (action >= 0 && action <= 6) ? actionNames[action] : "UNKNOWN";
        CI_LOGI("Received touch action %s pointer index %i", actionName, index);
        
        double timestamp = engine->cinderApp->getElapsedSeconds();
        if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            int pointerCount = AMotionEvent_getPointerCount(event);
            int pointerId = AMotionEvent_getPointerId(event, index);
            float x = AMotionEvent_getX(event, index);
            float y = AMotionEvent_getY(event, index);
            TouchEvent::Touch touch(ci::Vec2f(x, y), ci::Vec2f(x, y), pointerId, timestamp, NULL);
            
            touchState->activeTouches.erase(pointerId);
            touchState->activeTouches.insert(std::make_pair(pointerId, touch));
            touchState->touchesBegan.push_back(touch);
            CI_LOGI("Pointer id %d down x %f y %f", pointerId, x, y);
            
            engine_update_active_touches( cinderApp, engine->touchState );
            engine_update_touches_began( cinderApp, engine->touchState );
            
        }
        else if (action == AMOTION_EVENT_ACTION_MOVE) {
            int pointerCount = AMotionEvent_getPointerCount(event);
            for (int i=0; i < pointerCount; ++i) {
                int pointerId = AMotionEvent_getPointerId(event, i);
                float x = AMotionEvent_getX(event, i);
                float y = AMotionEvent_getY(event, i);
                map<int, TouchEvent::Touch>::iterator it = touchState->activeTouches.find(pointerId);
                
                if (it != touchState->activeTouches.end()) {
                    TouchEvent::Touch& prevTouch = it->second;
                    TouchEvent::Touch touch(ci::Vec2f(x, y), prevTouch.getPos(), pointerId, timestamp, NULL);
                    
                    touchState->activeTouches.erase(pointerId);
                    touchState->activeTouches.insert(std::make_pair(pointerId, touch));
                    touchState->touchesMoved.push_back(touch);
                    CI_LOGI("Pointer id %d move x %f y %f", pointerId, x, y);
                }
            }
            
            engine_update_active_touches( cinderApp, engine->touchState );
            engine_update_touches_moved( cinderApp, engine->touchState );
            
        }
        else if ( action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_CANCEL ) {
            int pointerCount = AMotionEvent_getPointerCount(event);
            int pointerId = AMotionEvent_getPointerId(event, index);
            float x = AMotionEvent_getX(event, index);
            float y = AMotionEvent_getY(event, index);
            
            touchState->activeTouches.erase(pointerId);
            touchState->touchesEnded.push_back(TouchEvent::Touch(ci::Vec2f(x, y), ci::Vec2f(x, y), pointerId, timestamp, NULL));
            CI_LOGI("Pointer id %d up x %f y %f", pointerId, x, y);
            
            engine_update_active_touches( cinderApp, engine->touchState );
            engine_update_touches_ended( cinderApp, engine->touchState );
            
        }
        
        return 1;
    }
    else if (eventType == AINPUT_EVENT_TYPE_KEY) {
        int32_t actionCode = AKeyEvent_getAction(event);
        int32_t keyCode = AKeyEvent_getKeyCode(event);
    }
    
    return 1; // consumes all input events
}





inline void engine_enable_accelerometer(struct engine* engine)
{
    if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                engine->accelerometerSensor);
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
            engine->accelerometerSensor, (1000L/engine->accelUpdateFrequency)*1000);
    }
}

inline void engine_disable_accelerometer(struct engine* engine)
{
    if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
            engine->accelerometerSensor);
    }
}

void log_engine_state(struct engine* engine) {
    static const char* activityStates[] = {
        "Start",
        "Resume",
        "Pause",
        "Stop",
        "Destroy"
    };
    CI_LOGD("Engine activity state: %s", activityStates[engine->activityState]);
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*) app->userData;
    ci::app::AppAndroid* cinderApp = engine->cinderApp;

    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // log_engine_state(engine);
            cinderApp->setSavedState(&(engine->androidApp->savedState), &(engine->androidApp->savedStateSize));
            break;

        case APP_CMD_INIT_WINDOW:
            // log_engine_state(engine);
            // The window is being shown, get it ready.
            if (engine->androidApp->window != NULL) {
                engine->orientation = cinderApp->orientationFromConfig();
                engine->cinderRenderer->setup(cinderApp, engine->androidApp, &(cinderApp->mWidth), &(cinderApp->mHeight));
                updateWindowSize(engine);
                cinderApp->privatePrepareSettings__();
                engine->animating = 0;

                //  New GL context, trigger app initialization
                engine->setupCompleted = false;
                engine->renewContext = true;
            }
            break;

        case APP_CMD_TERM_WINDOW:
            // log_engine_state(engine);
            // The window is being hidden or closed, clean it up.
            engine->animating = 0;
            engine->cinderRenderer->teardown();
            break;

        case APP_CMD_GAINED_FOCUS:
            // log_engine_state(engine);

            // Start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL && engine->accelEnabled) {
                engine_enable_accelerometer(engine);
            }

            if (!engine->setupCompleted) {
                if (engine->resumed) {
                    CI_LOGD("XXXXXX RESUMING privateResume__ renew context %s", engine->renewContext ? "true" : "false");
                    cinderApp->privateResume__(engine->renewContext);
                }
                else {
                    CI_LOGD("XXXXXX SETUP privateSetup__");
                    cinderApp->privateSetup__();
                }
                engine->cinderApp->privateResize__(ci::Vec2i( cinderApp->mWidth, cinderApp->mHeight ));
                engine->setupCompleted = true;
                engine->renewContext   = false;
                engine->resumed        = false;

                engine_draw_frame(engine);
            }

            engine->animating = 1;
            break;

        case APP_CMD_LOST_FOCUS:
            // log_engine_state(engine);
            //  Disable accelerometer (saves power)
            engine_disable_accelerometer(engine);
            engine->animating = 0;
            engine_draw_frame(engine);
            break;

        case APP_CMD_RESUME:
            engine->activityState = ACTIVITY_RESUME;
            // log_engine_state(engine);
            break;
        
        case APP_CMD_START:
            engine->activityState = ACTIVITY_START;
            // log_engine_state(engine);
            break;

        case APP_CMD_PAUSE:
            engine->activityState = ACTIVITY_PAUSE;
            cinderApp->privatePause__();
            engine->animating = 0;
            engine->resumed = true;
            engine_draw_frame(engine);
            // log_engine_state(engine);
            break;

        case APP_CMD_STOP:
            engine->activityState = ACTIVITY_STOP;
            // log_engine_state(engine);
            break;

        case APP_CMD_DESTROY:
            //  app has been destroyed, will crash if we attempt to do anything else
            engine->activityState = ACTIVITY_DESTROY;
            cinderApp->privateDestroy__();
            // log_engine_state(engine);
            break;

        case APP_CMD_CONFIG_CHANGED:
            engine->orientation = cinderApp->orientationFromConfig();
            break;

    }
}

static void android_ended(void* ptr)
{
    if (ptr) {
        // CI_LOGD("XXX Detach native thread");
        JavaVM* vm = (JavaVM*) ptr;
        vm->DetachCurrentThread();
    }
}

static void android_run(ci::app::AppAndroid* cinderApp, struct android_app* androidApp) 
{
    // Make sure glue isn't stripped.
    app_dummy();

    // XXX must free memory allocated for engine and touchState
    struct engine engine;
    memset(&engine, 0, sizeof(engine));

    engine.androidApp     = androidApp;
    engine.cinderApp      = cinderApp;
    engine.cinderRenderer = cinderApp->getRenderer();
    engine.touchState     = new TouchState;
    engine.accelEnabled   = false;
    engine.vm             = androidApp->activity->vm;

    JNIEnv* env;
    engine.vm->AttachCurrentThread(&env, NULL);

    pthread_key_t key;
    pthread_key_create(&key, android_ended);
    pthread_setspecific(key, engine.vm);

    //  Activity state tracking
    engine.savedState     = NULL;
    engine.setupCompleted = false;
    engine.resumed        = false;
    engine.renewContext   = true;

    //  XXX Used by accelerometer, move to cinder app?
    cinderApp->mEngine = &engine;

    androidApp->userData     = &engine;
    androidApp->onAppCmd     = engine_handle_cmd;
    androidApp->onInputEvent = engine_handle_input;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
           ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            androidApp->looper, LOOPER_ID_USER, NULL, NULL);

    // CI_LOGD("XXX android_run");

    if (androidApp->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        CI_LOGW("XXX android_run RESTORING SAVED STATE");
        engine.savedState = androidApp->savedState;
        // XXX currently restores via setup(), possibly better to use resume()?
        // engine.resumed = true;
    }

    //  Event loop
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(androidApp, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                        const float kGravity = 1.0f / 9.80665f;
                        cinderApp->privateAccelerated__(ci::Vec3f(-event.acceleration.x * kGravity, 
                                                                   event.acceleration.y * kGravity, 
                                                                   event.acceleration.z * kGravity));
                    }
                }
            }

            // Check if we are exiting.
            if (androidApp->destroyRequested != 0) {
                engine.animating = 0;
                engine.cinderRenderer->teardown();
                return;
            }
        }

        //  Update engine touch state
        // engine_update_touches(*cinderApp, engine.touchState); //  <----------------------- not polling for this anymore (handling this via event)

        if (engine.animating) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}

namespace cinder { namespace app {

AppAndroid::AppAndroid()
    : App()
{
    mLastAccel = mLastRawAccel = Vec3f::zero();
}

AppAndroid::~AppAndroid()
{
    CI_LOGW("~AppAndroid()");
    JNIEnv* env = getJNIEnv();
    env->DeleteGlobalRef(mClassLoader);
}

void AppAndroid::pause()
{
}

void AppAndroid::resume( bool renewContext )
{
    //  Override this to handle lost/recreated GL context
    //  You should recreate your GL context assets (textures/shaders) in this method
    if (renewContext) {
        setup();
    }
}

void AppAndroid::destroy()
{
}

void AppAndroid::setSavedState(void** state, size_t* size)
{
    *state = NULL;
    *size = 0;
}

void* AppAndroid::getSavedState()
{
    return mEngine->savedState;
}

fs::path AppAndroid::getInternalDataPath() const
{
    const char* path = (mAndroidApp && mAndroidApp->activity) ? 
        mAndroidApp->activity->internalDataPath : NULL;
    return path ? fs::path(path) : fs::path();
}

fs::path AppAndroid::getExternalDataPath() const
{
    const char* path = (mAndroidApp && mAndroidApp->activity) ? 
        mAndroidApp->activity->externalDataPath : NULL;
    return path ? fs::path(path) : fs::path();
}

int32_t AppAndroid::getSdkVersion()
{
    return (mAndroidApp && mAndroidApp->activity) ?
        mAndroidApp->activity->sdkVersion : -1;
}

void AppAndroid::setAndroidImpl( struct android_app* androidApp )
{
    mAndroidApp = androidApp;
}

void AppAndroid::launch( const char *title, int argc, char * const argv[] )
{
    clock_gettime(CLOCK_MONOTONIC, &mStartTime);
    android_run(this, mAndroidApp);
}

int	AppAndroid::getWindowWidth() const
{
    return mWidth;
}

int	AppAndroid::getWindowHeight() const
{
    return mHeight;
}

void AppAndroid::setWindowWidth( int windowWidth )
{
    mWidth = windowWidth;
}

void AppAndroid::setWindowHeight( int windowHeight )
{
    mHeight = windowHeight;
}

void AppAndroid::setWindowSize( int windowWidth, int windowHeight )
{
    setWindowWidth(windowWidth);
    setWindowHeight(windowHeight);
}

//! Enables the accelerometer
void AppAndroid::enableAccelerometer( float updateFrequency, float filterFactor )
{
    if ( mEngine->accelerometerSensor != NULL ) {
        mAccelFilterFactor = filterFactor;

        if( updateFrequency <= 0 )
            updateFrequency = 30.0f;

        mEngine->accelUpdateFrequency = updateFrequency;

        if ( !mEngine->accelEnabled )
            engine_enable_accelerometer(mEngine);

        mEngine->accelEnabled = true;
    }
}

void AppAndroid::disableAccelerometer() {
    if ( mEngine->accelerometerSensor != NULL && mEngine->accelEnabled ) {
        mEngine->accelEnabled = false;
        engine_disable_accelerometer(mEngine);
    }
}

//! Returns the maximum frame-rate the App will attempt to maintain.
float AppAndroid::getFrameRate() const
{
    return 0;
}

//! Sets the maximum frame-rate the App will attempt to maintain.
void AppAndroid::setFrameRate( float aFrameRate )
{
}

//! Returns whether the App is in full-screen mode or not.
bool AppAndroid::isFullScreen() const
{
    return true;
}

//! Sets whether the active App is in full-screen mode based on \a fullScreen
void AppAndroid::setFullScreen( bool aFullScreen )
{
}

double AppAndroid::getElapsedSeconds() const
{
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    return ( (currentTime.tv_sec + currentTime.tv_nsec / 1e9) 
            - (mStartTime.tv_sec + mStartTime.tv_nsec / 1e9) );
}

fs::path AppAndroid::getAppPath()
{ 
    // XXX TODO
    return fs::path();
}

void AppAndroid::quit()
{
    return;
}

Orientation_t AppAndroid::orientation()
{
    return mEngine->orientation;
}

void AppAndroid::privatePrepareSettings__()
{
    prepareSettings( &mSettings );
}

void AppAndroid::privatePause__()
{
    pause();
}

void AppAndroid::privateResume__( bool renewContext ) 
{
    resume(renewContext);
}

void AppAndroid::privateDestroy__()
{
    destroy();
}

void AppAndroid::privateTouchesBegan__( const TouchEvent &event )
{
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesBegan.begin(); ( cbIter != mCallbacksTouchesBegan.end() ) && ( ! handled ); ++cbIter )
        handled = (cbIter->second)( event );		
    if( ! handled )	
        touchesBegan( event );
}

void AppAndroid::privateTouchesMoved__( const TouchEvent &event )
{	
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesMoved.begin(); ( cbIter != mCallbacksTouchesMoved.end() ) && ( ! handled ); ++cbIter )
        handled = (cbIter->second)( event );		
    if( ! handled )	
        touchesMoved( event );
}

void AppAndroid::privateTouchesEnded__( const TouchEvent &event )
{	
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesEnded.begin(); ( cbIter != mCallbacksTouchesEnded.end() ) && ( ! handled ); ++cbIter )
        handled = (cbIter->second)( event );		
    if( ! handled )	
        touchesEnded( event );
}

void AppAndroid::privateAccelerated__( const Vec3f &direction )
{
    Vec3f filtered = mLastAccel * (1.0f - mAccelFilterFactor) + direction * mAccelFilterFactor;

    AccelEvent event( filtered, direction, mLastAccel, mLastRawAccel );

    bool handled = false;
    for( CallbackMgr<bool (AccelEvent)>::iterator cbIter = mCallbacksAccelerated.begin(); ( cbIter != mCallbacksAccelerated.end() ) && ( ! handled ); ++cbIter )
        handled = (cbIter->second)( event );		
    if( ! handled )	
        accelerated( event );

    mLastAccel = filtered;
    mLastRawAccel = direction;
}

Orientation_t AppAndroid::orientationFromConfig()
{
    Orientation_t ret = ORIENTATION_ANY;
    int orient = AConfiguration_getOrientation(mAndroidApp->config);

    switch (orient) {
        case ACONFIGURATION_ORIENTATION_PORT:
            ret = ORIENTATION_PORTRAIT;
            break;
        case ACONFIGURATION_ORIENTATION_LAND:
            ret = ORIENTATION_LANDSCAPE;
            break;
        case ACONFIGURATION_ORIENTATION_SQUARE:
            ret = ORIENTATION_SQUARE;
            break;
        default:
            break;
    }

    return ret;
}


#if defined( CINDER_AASSET )

//  static loadResource method, loads from assets/
DataSourceAssetRef AppAndroid::loadResource(const std::string &resourcePath)
{
    AppAndroid* cinderApp = AppAndroid::get();
    CI_LOGW("cinderApp %p", cinderApp);
    if (cinderApp) {
        CI_LOGW("loading via manager");
        AAssetManager* mgr = cinderApp->mAndroidApp->activity->assetManager;
        return DataSourceAsset::create(mgr, resourcePath);
    }
    else {
        throw ResourceLoadExc( resourcePath );
    }
}

bool AppAndroid::hasResource(const fs::path& assetPath)
{
    AppAndroid* cinderApp = AppAndroid::get();
    AAssetManager* mgr = cinderApp->mAndroidApp->activity->assetManager;
    AAsset* asset = AAssetManager_open(mgr, assetPath.string().c_str(), AASSET_MODE_STREAMING);
    bool hasResource = bool(asset);
    AAsset_close(asset);
    return hasResource;
}

void AppAndroid::copyResource(const fs::path& assetPath, const fs::path& destDir, bool overwrite)
{
    AppAndroid* cinderApp = AppAndroid::get();
    AAssetManager* mgr = cinderApp->mAndroidApp->activity->assetManager;

    if (assetPath.empty() || destDir.empty()) {
        CI_LOGE("copyResource: Missing assetPath or destination dir");
        return;
    }

    // CI_LOGD("Copying resource %s to %s", assetPath.string().c_str(), destDir.string().c_str());
    fs::path outPath = destDir / assetPath.filename();

    if (fs::exists(outPath) && !overwrite)
        return;

    {
        // XXX fix createParentDirs in writeFileStream
        OStreamFileRef os = writeFileStream(outPath);
        if (!os) {
            return;
        }
        AAsset* asset = AAssetManager_open(mgr, assetPath.string().c_str(), AASSET_MODE_STREAMING);

        const int BUFSIZE = 8192;
        unsigned char buf[BUFSIZE];
        int readSize;

        while (true) {
            readSize = AAsset_read(asset, (void *) buf, BUFSIZE);
            if (readSize > 0) {
                os->writeData(buf, readSize);
            }
            else {
                break;
            }
        }
        AAsset_close(asset);
    }
}

void AppAndroid::copyResourceDir(const fs::path& assetPath, const fs::path& destDir, bool overwrite)
{
    AppAndroid* cinderApp = AppAndroid::get();
    AAssetManager* mgr = cinderApp->mAndroidApp->activity->assetManager;

    // XXX Untested
    AAssetDir* dir = AAssetManager_openDir(mgr, assetPath.string().c_str());
    fs::path outDir = destDir / assetPath.filename();
    if (!fs::exists(outDir)) {
        fs::create_directory(outDir);
    }

    char* filename = NULL;
    while (true) {
        const char* filename = AAssetDir_getNextFileName(dir);
        if (filename == NULL) {
            break;
        }
        copyResource(filename, outDir, overwrite);
    }
    AAssetDir_close(dir);
}

JavaVM* AppAndroid::getJavaVM()
{
    return mEngine->vm;
}

JNIEnv* AppAndroid::getJNIEnv()
{
    JNIEnv* env = 0;
    int err = mEngine->vm->GetEnv((void**) &env, JNI_VERSION_1_4);
    if (err == JNI_EDETACHED) {
        CI_LOGE("getJNIEnv error: current thread not attached to Java VM");
    }
    else if (err == JNI_EVERSION) {
        CI_LOGE("getJNIEnv error: VM doesn't support requested JNI version");
    }
    return env;
}

#define MAX_LOCAL_REFS 20
void AppAndroid::initJNI()
{
    JNIEnv* env = getJNIEnv();
    env->PushLocalFrame(MAX_LOCAL_REFS);
    jclass activityClass     = env->FindClass("android/app/NativeActivity");
    jmethodID getClassLoader = env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jclass loader            = env->FindClass("java/lang/ClassLoader");

    mClassLoader             = env->NewGlobalRef(env->CallObjectMethod(mAndroidApp->activity->clazz, getClassLoader));
    mFindClassMID            = env->GetMethodID(loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    env->PopLocalFrame(NULL);
}

jclass AppAndroid::findClass(const char* className)
{
    JNIEnv* env = getJNIEnv();
    jstring strClassName  = env->NewStringUTF(className);
    jclass theClass = static_cast<jclass>(env->CallObjectMethod(mClassLoader, mFindClassMID, strClassName));
    env->DeleteLocalRef(strClassName);

    //  Returns a local class reference, that will have to be deleted by the caller
    return theClass;
}

jobject AppAndroid::getActivity()
{
    return mAndroidApp->activity->clazz;
}

#endif

} } // namespace cinder::app
