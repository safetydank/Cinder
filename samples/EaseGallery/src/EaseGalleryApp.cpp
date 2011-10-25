#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Easing.h"
#include "cinder/Text.h"

#if defined( CINDER_GLES2 )
#include "cinder/gl/gles2.h"
#endif

#if defined( CINDER_ANDROID )
#include "cinder/gl/TextureFont.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

const float TWEEN_SPEED = 0.5f;

struct EaseBox {
  public:
	EaseBox( std::function<float (float)> fn, string name )
		: mFn( fn )
	{
#if ! defined( CINDER_ANDROID )
		// create label
		TextLayout text; text.clear( Color::white() ); text.setColor( Color(0.5f, 0.5f, 0.5f) );
		try { text.setFont( Font( "Futura-CondensedMedium", 18 ) ); } catch( ... ) { text.setFont( Font( "Arial", 18 ) ); }
		text.addLine( name );
		mLabelTex = gl::Texture( text.render( true ) );
#else
        if (!sTexFont)
            sTexFont = gl::TextureFont::create( Font( loadFile("/system/fonts/DroidSans.ttf"), 18 ) );
        mName = name;
#endif
	}
	
	void draw( float t ) const
	{
		// draw box and frame
		gl::color( Color( 1.0f, 1.0f, 1.0f ) );
		gl::drawSolidRect( mDrawRect );
		gl::color( Color( 0.4f, 0.4f, 0.4f ) );
		gl::drawStrokedRect( mDrawRect );
		gl::color( Color::white() );
#if ! defined( CINDER_ANDROID )
		gl::draw( mLabelTex, mDrawRect.getCenter() - mLabelTex.getSize() / 2 );
#else
        gl::color( Color::black() );
        Vec2f labelSize = sTexFont->measureString( mName );
        Vec2f baseline;
        baseline.x = (mDrawRect.getWidth() - labelSize.x) / 2.0f + mDrawRect.x1;
        float ascent = sTexFont->getFont().getAscent();
        baseline.y = mDrawRect.y1 + (mDrawRect.y2 - mDrawRect.y1 - ascent) / 2.0f + ascent;
        sTexFont->drawString( mName, baseline );
#endif
				
		// draw graph
#if ! defined( CINDER_ANDROID )
		gl::color( ColorA( 0.25f, 0.5f, 1.0f, 0.5f ) );
		glBegin( GL_LINE_STRIP );
		for( float x = 0; x < mDrawRect.getWidth(); x += 0.25f ) {
			float y = 1.0f - mFn( x / mDrawRect.getWidth() );
			gl::vertex( Vec2f( x, y * mDrawRect.getHeight() ) + mDrawRect.getUpperLeft() );
		}
		glEnd();
#endif
		
		// draw animating circle
		gl::color( Color( 1, 0.5f, 0.25f ) );
		gl::drawSolidCircle( mDrawRect.getUpperLeft() + mFn( t ) * mDrawRect.getSize(), 5.0f );
	}
	
	std::function<float (float)>	mFn;
	Rectf							mDrawRect;
	gl::Texture						mLabelTex;
#if defined( CINDER_ANDROID )
    static gl::TextureFontRef  sTexFont;
    string mName;
#endif
};

#if defined( CINDER_ANDROID )
gl::TextureFontRef EaseBox::sTexFont;
#endif

class EaseGalleryApp : public AppNative {
  public:
	void setup();
	void draw();
	void resize( ResizeEvent event );
	void sizeRectangles();
	
	vector<EaseBox>		mEaseBoxes;
#if defined( CINDER_GLES2 )
    gl::GlesContextRef  mContext;
#endif
};

void EaseGalleryApp::setup()
{
#if ! defined( CINDER_GLES2 )
	setWindowSize( 950, 800 );
#else
    EaseBox::sTexFont = gl::TextureFontRef();
    mContext = gl::setGlesContext();
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
#endif

	mEaseBoxes.push_back( EaseBox( EaseInQuad(), "EaseInQuad" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutQuad(), "EaseOutQuad" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutQuad(), "EaseInOutQuad" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInQuad(), "EaseOutInQuad" ) );

	mEaseBoxes.push_back( EaseBox( EaseInCubic(), "EaseInCubic" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutCubic(), "EaseOutCubic" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutCubic(), "EaseInOutCubic" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInCubic(), "EaseOutInCubic" ) );

	mEaseBoxes.push_back( EaseBox( EaseInQuart(), "EaseInQuart" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutQuart(), "EaseOutQuart" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutQuart(), "EaseInOutQuart" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInQuart(), "EaseOutInQuart" ) );

	mEaseBoxes.push_back( EaseBox( EaseInQuint(), "EaseInQuint" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutQuint(), "EaseOutQuint" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutQuint(), "EaseInOutQuint" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInQuint(), "EaseOutInQuint" ) );

	mEaseBoxes.push_back( EaseBox( EaseInSine(), "EaseInSine" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutSine(), "EaseOutSine" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutSine(), "EaseInOutSine" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInSine(), "EaseOutInSine" ) );

	mEaseBoxes.push_back( EaseBox( EaseInExpo(), "EaseInExpo" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutExpo(), "EaseOutExpo" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutExpo(), "EaseInOutExpo" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInExpo(), "EaseOutInExpo" ) );

	mEaseBoxes.push_back( EaseBox( EaseInCirc(), "EaseInCirc" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutCirc(), "EaseOutCirc" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutCirc(), "EaseInOutCirc" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInCirc(), "EaseOutInCirc" ) );

	mEaseBoxes.push_back( EaseBox( EaseInAtan(), "EaseInAtan" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutAtan(), "EaseOutAtan" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutAtan(), "EaseInOutAtan" ) );
	mEaseBoxes.push_back( EaseBox( EaseNone(), "EaseNone" ) );

	mEaseBoxes.push_back( EaseBox( EaseInBack(), "EaseInBack" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutBack(), "EaseOutBack" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutBack(), "EaseInOutBack" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInBack(), "EaseOutInBack" ) );

	mEaseBoxes.push_back( EaseBox( EaseInBounce(), "EaseInBounce" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutBounce(), "EaseOutBounce" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutBounce(), "EaseInOutBounce" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInBounce(), "EaseOutInBounce" ) );

	mEaseBoxes.push_back( EaseBox( EaseInElastic( 2, 1 ), "EaseInElastic(2, 1)" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutElastic( 1, 4 ), "EaseOutElastic(1, 4)" ) );
	mEaseBoxes.push_back( EaseBox( EaseInOutElastic( 2, 1 ), "EaseInOutElastic( 2, 1 )" ) );
	mEaseBoxes.push_back( EaseBox( EaseOutInElastic( 1, 4 ), "EaseOutInElastic( 4, 1 )" ) );

	sizeRectangles();
}

void EaseGalleryApp::resize( ResizeEvent event )
{
	sizeRectangles();
}

void EaseGalleryApp::sizeRectangles()
{
	const int cellColumns = 4;
	const int cellRows = 11;	
	const Vec2f padding( 10, 10 );
	const Vec2f cellSize( ( getWindowWidth() - padding.x ) / cellColumns - padding.x, ( getWindowHeight() - padding.y ) / cellRows - padding.y );

	for( size_t c = 0; c < mEaseBoxes.size(); ++c ) {
		int col = c % cellColumns;
		int row = c / cellColumns;
		mEaseBoxes[c].mDrawRect = Rectf( Vec2f( col, row ) * (cellSize + padding), Vec2f( col, row ) * (cellSize + padding) + cellSize ) + padding;
	}
}

void EaseGalleryApp::draw()
{
#if defined( CINDER_GLES2 )
    mContext->bind();
#endif

	gl::clear( Color( 0.9f, 0.9f, 0.9f ) ); 
	gl::enableAlphaBlending();
	glLineWidth( 2.0f );

	// time cycles every 1 / TWEEN_SPEED seconds, with a 50% pause at the end
	float time = math<float>::clamp( fmod( getElapsedSeconds() * TWEEN_SPEED, 1 ) * 1.5f, 0, 1 );
	for( vector<EaseBox>::iterator easeIt = mEaseBoxes.begin(); easeIt != mEaseBoxes.end(); ++easeIt )
		easeIt->draw( time );
}


CINDER_APP_NATIVE( EaseGalleryApp, RendererGl(3) )
