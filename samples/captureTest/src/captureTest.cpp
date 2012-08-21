#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Utilities.h"

#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/AppCocoaTouch.h"
	typedef ci::app::AppCocoaTouch AppBase;
#elif defined( CINDER_ANDROID )
	#include "cinder/app/AppNative.h"
	typedef ci::app::AppNative AppBase;
#else
	#include "cinder/app/AppBasic.h"
	typedef ci::app::AppBasic AppBase;
#endif

#include "cinder/Capture.h"
#include <map>

using namespace ci;
using namespace ci::app;

class CaptureApp : public AppBase {
 public:	
	void setup();
	void keyDown( KeyEvent event );
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	
	void update();
	void draw();

	#if defined( CINDER_ANDROID )
    void resume( bool renewContext );
	#endif

	Capture				mCapture;
	gl::Texture			mTexture;

	FontRef				mFont;
	gl::TextureFontRef	mTextureFont;
};

void CaptureApp::setup()
{	
	console() << "CaptureApp::setup" << std::endl;
	//
	//try {
	const std::vector<Capture::DeviceRef> devs = Capture::getDevices(true);
		mCapture = Capture( 640, 480, devs.at(0) );
		//mCapture.start();
	/*}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
	*/
	mFont = Font::getDefault();
	mTextureFont = gl::TextureFont::create( mFont );
}

#if defined( CINDER_ANDROID )
void CaptureApp::resume(bool renewContext)
{
    if (renewContext) {
        //  Release GL resources
        mTexture.reset();
        mTextureFont.reset();
        //  Recreate GL resources
        mTextureFont = gl::TextureFont::create( mFont );
    }
}
#endif

void CaptureApp::keyDown( KeyEvent event )
{
#if defined( CINDER_MAC )
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' )
		( mCapture && mCapture.isCapturing() ) ? mCapture.stop() : mCapture.start();
#endif
}

void CaptureApp::update()
{
	if( mCapture && mCapture.checkNewFrame() ) {
		const Surface8u surf = mCapture.getSurface();
		
		mTexture = gl::Texture( surf );
	}
}

void CaptureApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );

	// Draw FPS
	gl::color( Color::white() );
	mTextureFont->drawString( toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );
	
	if( mTexture ) {
		glPushMatrix();
#if defined( CINDER_COCOA_TOUCH )
			//change iphone to landscape orientation
			glRotatef(90, 0.0, 0.0, 1.0);
			glTranslatef(0.0f, -320.0f, 0.0f );
#endif
			gl::draw( mTexture );
		glPopMatrix();
		
	}
}


#if defined( CINDER_COCOA_TOUCH )
CINDER_APP_COCOA_TOUCH( CaptureApp, RendererGl )
#elif defined( CINDER_ANDROID )
CINDER_APP_NATIVE( CaptureApp, RendererGl(0) )
#else
CINDER_APP_BASIC( CaptureApp, RendererGl )
#endif
