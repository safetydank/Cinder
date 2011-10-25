#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;

class iPhoneAccelerometerApp : public AppNative {
  public:
	virtual void	setup();
	virtual void	accelerated( AccelEvent event );
	virtual void	draw();
	
	Matrix44f		mModelView;
	CameraPersp		mCam;
};

void iPhoneAccelerometerApp::setup()
{
	enableAccelerometer();
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	mCam.lookAt( Vec3f( 0, 0, 3 ), Vec3f::zero() );	
    glEnable(GL_CULL_FACE);
}

void iPhoneAccelerometerApp::accelerated( AccelEvent event )
{
	mModelView = event.getMatrix();
	if( event.isShake() )
		console() << "Shake!" << std::endl;
}

void iPhoneAccelerometerApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enableDepthRead();

	gl::setMatrices( mCam );
	gl::multModelView( mModelView );
	gl::drawColorCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
}

CINDER_APP_NATIVE( iPhoneAccelerometerApp, RendererGl )
