#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

#include "cinder/ImageIo.h"
#include "cinder/DataSource.h"
#include "cinder/Stream.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"

#include "cinder/Font.h"

#if defined( CINDER_GLES2 )
#include "cinder/gl/gles2.h"
#endif

using namespace ci;
using namespace ci::app;

using namespace std;

class AndroidTest : public AppNative {
public:
	void setup();
	void draw();
	void resume(bool renewContext);
	
	// gl::Texture		   mTexture;	

	// Font               mFont1;
	// Font               mFont2;
	// Font               mFont3;

	// gl::TextureFontRef mTexFont1;
	// gl::TextureFontRef mTexFont2;
	// gl::TextureFontRef mTexFont3;

	// gl::Texture        mFontTexture;
    gl::Texture mLogoTexture;

#if defined( CINDER_GLES2 )
    gl::GlesContextRef mContext;
#endif
};

void AndroidTest::resume(bool renewContext)
{
    if (renewContext) {
        mLogoTexture.reset();
        mContext.reset();

        setup();
    }
}

void AndroidTest::setup()
{
    // CI_LOGW("XXX AndroidTest::setup");
	// try {
	// 	console() << "Loading logo image" << endl;
	// 	mTexture = gl::Texture( loadImage( loadResource("cinder_logo.png") ) );
	// 	console() << "success!!!" << endl;
	// }
	// catch( ... ) {
	// 	console() << "unable to load the texture file!" << std::endl;
	// }

    // mFont1 = Font(loadFile("/system/fonts/DroidSerif-Italic.ttf"), 40);
    // mFont2 = Font(loadFile("/system/fonts/DroidSerif-Regular.ttf"), 50);
    // mFont3 = Font(loadFile("/system/fonts/DroidSans.ttf"), 20);
	// console() << "Loaded font name " << mFont1.getName() << " num glyphs " << mFont1.getNumGlyphs() << endl;

    // gl::TextureFont::Format format;

    // format.textureWidth(512);
    // format.textureHeight(512);
    // format.premultiply(true);

    // gl::TextureFont::Atlas atlas(format);

    // mTexFont3 = gl::TextureFont::create( mFont3, atlas );
    // mTexFont1 = gl::TextureFont::create( mFont1, atlas );
    // mTexFont2 = gl::TextureFont::create( mFont2, atlas );

    // console() << "XXX font tex count font1 " << mTexFont1->getTextures().size() << " font 2 "
    //     << mTexFont2->getTextures().size() << endl;

    // mFontTexture = mTexFont2->getTextures().front();
    // gl::enableAlphaBlending(format.getPremultiply());

    DataSourceRef dataSource = loadResource("cinder_logo.png");
    if (!dataSource) {
        CI_LOGW("Error loading resource");
    }
    else {
        mLogoTexture = gl::Texture(loadImage(dataSource));
    }

#if defined( CINDER_GLES2 )
    mContext = gl::setGlesContext();
#endif
}

void AndroidTest::draw()
{
#if defined( CINDER_GLES2 )
    mContext->bind();
#endif

    float r = math<float>::abs(math<float>::sin(getElapsedSeconds() * 0.30f));
    float g = math<float>::abs(math<float>::cos(getElapsedSeconds() * 0.17f));
    float b = math<float>::abs(math<float>::cos(getElapsedSeconds() * 0.67f));
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color(r,g,b) );

//    int rightEdge = getWindowWidth();
//	float helloWidth = mTexFont1->measureString( "Hello" ).x;
//    mTexFont1->drawString( "Hello", Vec2f(rightEdge - helloWidth, mTexFont1->getAscent()) );
//    float worldWidth = mTexFont2->measureString( "World" ).x;
//    mTexFont2->drawString( "World", Vec2f(rightEdge - worldWidth, 
//                mTexFont1->getAscent() + mTexFont1->getDescent() + mTexFont2->getAscent()) );
//    float demoWidth = mTexFont3->measureString( "Font packing demonstration" ).x;
//    mTexFont3->drawString( "Font packing demonstration", Vec2f(rightEdge - demoWidth,
//                getWindowHeight() - mTexFont3->getDescent()) );
//
	// if( mFontTexture ) {
	// 	gl::draw( mFontTexture, Vec2f( 0, 0 ) );
    // }
    if ( mLogoTexture )
        gl::draw( mLogoTexture, Vec2f(0, 0) );
#if defined( CINDER_GLES2 )
    mContext->unbind();
#endif
}

CINDER_APP_NATIVE( AndroidTest, RendererGl )

