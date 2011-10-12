#if ! defined( CINDER_GLES2 )

void GLContext::initialize()
{
    //  Do nothing 
}

void setModelView( const Camera &cam )
{
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( cam.getModelViewMatrix().m );
}

void setProjection( const Camera &cam )
{
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( cam.getProjectionMatrix().m );
}

void setMatrices( const Camera &cam )
{
	setProjection( cam );
	setModelView( cam );
}

void pushModelView()
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
}

void popModelView()
{
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void pushModelView( const Camera &cam )
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadMatrixf( cam.getModelViewMatrix().m );
}

void pushProjection( const Camera &cam )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadMatrixf( cam.getProjectionMatrix().m );
}

void pushMatrices()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();	
}

void popMatrices()
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void multModelView( const Matrix44f &mtx )
{
	glMatrixMode( GL_MODELVIEW );
	glMultMatrixf( mtx );
}

void multProjection( const Matrix44f &mtx )
{
	glMatrixMode( GL_PROJECTION );
	glMultMatrixf( mtx );
}

Matrix44f getModelView()
{
	Matrix44f result;
	glGetFloatv( GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat*>( &(result.m) ) );
	return result;
}

Matrix44f getProjection()
{
	Matrix44f result;
	glGetFloatv( GL_PROJECTION_MATRIX, reinterpret_cast<GLfloat*>( &(result.m) ) );
	return result;
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );

	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( cam.getProjectionMatrix().m );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( cam.getModelViewMatrix().m );
	if( originUpperLeft ) {
		glScalef( 1.0f, -1.0f, 1.0f );           // invert Y axis so increasing Y goes down.
		glTranslatef( 0.0f, (float)-screenHeight, 0.0f );       // shift origin up to upper-left corner.
		glViewport( 0, 0, screenWidth, screenHeight );
	}
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
#if defined( CINDER_GLES1 )
	if( originUpperLeft )
		glOrthof( 0, screenWidth, screenHeight, 0, -1.0f, 1.0f );
	else
		glOrthof( 0, screenWidth, 0, screenHeight, -1.0f, 1.0f );
#else	
	if( originUpperLeft )
		glOrtho( 0, screenWidth, screenHeight, 0, -1.0f, 1.0f );
	else
		glOrtho( 0, screenWidth, 0, screenHeight, -1.0f, 1.0f );
#endif
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport( 0, 0, screenWidth, screenHeight );
}

void translate( const Vec2f &pos )
{
	glTranslatef( pos.x, pos.y, 0 );
}

void translate( const Vec3f &pos )
{
	glTranslatef( pos.x, pos.y, pos.z );
}

void scale( const Vec3f &scale )
{
	glScalef( scale.x, scale.y, scale.z );
}

void rotate( const Vec3f &xyz )
{
	glRotatef( xyz.x, 1.0f, 0.0f, 0.0f );
	glRotatef( xyz.y, 0.0f, 1.0f, 0.0f );
	glRotatef( xyz.z, 0.0f, 0.0f, 1.0f );
}

void rotate( const Quatf &quat )
{
	Vec3f axis;
	float angle;
	quat.getAxisAngle( &axis, &angle );
	if( math<float>::abs( angle ) > EPSILON_VALUE )
		glRotatef( toDegrees( angle ), axis.x, axis.y, axis.z );
}

void enableAlphaTest( float value, int func )
{
	glEnable( GL_ALPHA_TEST );
	glAlphaFunc( func, value );
}

void disableAlphaTest()
{
	glDisable( GL_ALPHA_TEST );
}

#if ! defined( CINDER_GLES1 )
void enableWireframe()
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void disableWireframe()
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
#endif

#endif // ! defined( CINDER_GLES2 )

#if defined( CINDER_GLES2 )

//  Static members

static GLES2ContextRef sContext;

void initGLES2()
{
    sContext = GLES2ContextRef(new GLES2Context());
}

void freeGLES2()
{
    sContext = GLES2ContextRef();
}

void bindGLES2()
{
    sContext->bind();
}

void unbindGLES2()
{
    sContext->unbind();
}

class CinderProgES2 : public GlslProg
{
public:
    static const char* verts; 
    static const char* frags; 

    CinderProgES2() : GlslProg(verts, frags) 
    { }
};

const char* CinderProgES2::verts = 
        "attribute vec3 aPosition;\n"
        "attribute vec4 aColor;\n"
        "attribute vec2 aTexCoord;\n"

        "uniform mat4 uProjection;\n"
        "uniform mat4 uModelView;\n"
        "uniform vec4 uVertexColor;\n"

        "uniform bool uHasVertexAttr;\n"
        "uniform bool uHasTexCoordAttr;\n"
        "uniform bool uHasColorAttr;\n"
        "uniform bool uHasNormalAttr;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "  vColor = uVertexColor;\n"
        "  if (uHasColorAttr) {\n"
        "    vColor *= aColor;\n"
        "  }\n"
        "  if (uHasTexCoordAttr) {\n"
        "    vTexCoord = aTexCoord;\n"
        "  }\n"
        "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
        "}\n";

const char* CinderProgES2::frags = 
        "precision mediump float;\n"

        "uniform bool uHasTexCoordAttr;\n"
        "uniform sampler2D sTexture;\n"

        "varying vec4 vColor;\n"
        "varying vec2 vTexCoord;\n"

        "void main() {\n"
        "    if (uHasTexCoordAttr) {\n"
        "      gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
        "    }\n"
        "    else {\n"
        "      gl_FragColor = vColor;\n"
        "    }\n"
        "}\n";

// GlesContextRef GlesContext::create()
// {
//     return GlesContextRef(new GlesContext());
// }
// 
// GlesContextRef GlesContext::create(GlslProg& shader, GlesAttr& attr)
// {
//     return GlesContextRef(new GlesContext(shader, attr));
// }

GlesContext::GlesContext()
{
    CI_LOGW("Initializing CinderProgES2 shader");

	try {
        // CinderProgES2 shader;
        GlslProg shader(CinderProgES2::verts, CinderProgES2::frags);
        GlesAttr attr(shader.getAttribLocation("aPosition"),
                      shader.getAttribLocation("aTexCoord"),
                      shader.getAttribLocation("aColor"),
                      shader.getAttribLocation("aNormal"));
        attr.mTexSampler = shader.getUniformLocation("sTexture");
        init(shader, attr);
    }
    catch( gl::GlslProgCompileExc &exc ) {
        CI_LOGW("Shader compile error: \n");
        CI_LOGW("%s\n", exc.what());
    }
    catch( ... ) {
        CI_LOGW("Unable to load shader\n");
    }
}

GlesContext::GlesContext(GlslProg& shader, GlesAttr& attr)
{
    init(shader, attr);
}

void GlesContext::init(GlslProg& shader, GlesAttr& attr)
{
    //  XXX use a setter
    mProg = shader;
    mAttr = attr;
    mBound = false;
    mAttr.mSelectAttr = this;
    mColor = ColorA::white();
    mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = true;
}

void GlesContext::bind()
{
    if ( ! mBound ) {
        mBound = true;
        mProg.bind();
    }
    updateUniforms();
}

void GlesContext::unbind()
{
    if ( mBound )
        mProg.unbind();

    mBound = false;
    mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = true;
    mActiveAttrs = 0;
}

void GlesContext::selectAttrs(uint32_t activeAttrs)
{
    if (mActiveAttrs != activeAttrs) {
        // CI_LOGW("XXX New attributes set: %d", activeAttrs);
        mActiveAttrsDirty = true;
    }
    mActiveAttrs = activeAttrs;
    updateUniforms();
}

GlesAttr& GlesContext::attr()
{
    return mAttr;
}

void GlesContext::setMatrices( const Camera &cam )
{
    mModelView = cam.getModelViewMatrix();
    mModelViewDirty = true;
    mProj = cam.getProjectionMatrix();
    mProjDirty = true;
    updateUniforms();
}

void GlesContext::setModelView( const Camera &cam )
{
    mModelView = cam.getModelViewMatrix();
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::setProjection( const Camera &cam )
{
    mProj = cam.getProjectionMatrix();
    mProjDirty = true;
    updateUniforms();
}

void GlesContext::setProjection( const ci::Matrix44f &proj )
{
    mProj = proj;
    mProjDirty = true;
    updateUniforms();
}

void GlesContext::pushModelView()
{
    mModelViewStack.push_back(mModelView);
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::popModelView()
{
    if (!mModelViewStack.empty()) {
        mModelView = mModelViewStack.back();
        mModelViewStack.pop_back();
        mModelViewDirty = true;
        updateUniforms();
    }
}

void GlesContext::pushModelView( const Camera &cam )
{
    mModelViewStack.push_back(cam.getModelViewMatrix().m);
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::pushProjection( const Camera &cam )
{
    mProjStack.push_back(mProj);
    mProjDirty = true;
    updateUniforms();
}

void GlesContext::pushMatrices()
{
    mModelViewStack.push_back(mModelView);
    mModelViewDirty = true;
    mProjStack.push_back(mProj);
    mProjDirty = true;
    updateUniforms();
}

void GlesContext::popMatrices()
{
    if (!mModelViewStack.empty()) {
        mModelView = mModelViewStack.back();
        mModelViewStack.pop_back();
        mModelViewDirty = true;
    }
    if (!mProjStack.empty()) {
        mProj = mProjStack.back();
        mProjStack.pop_back();
        mProjDirty = true;
    }

    if (mModelViewDirty || mProjDirty)
        updateUniforms();
}

void GlesContext::multModelView( const Matrix44f &mtx )
{
    mModelView *= mtx;
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::multProjection( const Matrix44f &mtx )
{
    mProj *= mtx;
    mProjDirty = true;
    updateUniforms();
}

Matrix44f GlesContext::getModelView()
{
    return mModelView;
}

Matrix44f GlesContext::getProjection()
{
    return mProj;
}

void GlesContext::setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );

    mProj = cam.getProjectionMatrix();
    mModelView = cam.getModelViewMatrix();

	if( originUpperLeft ) {
        scale(Vec3f(1.0f, -1.0f, 1.0f));  // invert Y axis so increasing Y goes down.
        translate(1.0f, -1.0f, 1.0f);     // shift origin up to upper-left corner.
		glViewport( 0, 0, screenWidth, screenHeight );
	}

    mProjDirty = mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft)
{
    CameraOrtho cam;

    if (originUpperLeft) {
        cam.setOrtho(0, screenWidth, screenHeight, 0, 1.0f, -1.0f);
    }
    else {
        cam.setOrtho(0, screenWidth, 0, screenHeight, 1.0f, -1.0f);
    }

    mProj = cam.getProjectionMatrix();
    mModelView = Matrix44f::identity();
    glViewport( 0, 0, screenWidth, screenHeight );

    mProjDirty = mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::translate( const Vec2f &pos )
{
    mModelView.translate(Vec3f(pos.x, pos.y, 0));
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::translate( const Vec3f &pos )
{
    mModelView.translate(pos);
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::scale( const Vec3f &scl )
{
    mModelView.scale(scl);
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::rotate( const Vec3f &xyz )
{
    Vec3f xyzrad(toRadians(xyz.x), toRadians(xyz.y), toRadians(xyz.z));
    mModelView.rotate(xyzrad);
    mModelViewDirty = true;
    updateUniforms();
}

void GlesContext::rotate( const Quatf &quat )
{
	Vec3f axis;
	float angle;
	quat.getAxisAngle( &axis, &angle );
    if( math<float>::abs( angle ) > EPSILON_VALUE ) {
		mModelView.rotate( Vec3f(axis.x, axis.y, axis.z), angle );
        mModelViewDirty = true;
        updateUniforms();
    }
}

void GlesContext::color( float r, float g, float b )
{
    mColor = ColorA( r, g, b, 1.0f );
    mColorDirty = true;
    updateUniforms();
}

void GlesContext::color( float r, float g, float b, float a )
{
    mColor = ColorA( r, g, b, a );
    mColorDirty = true;
    updateUniforms();
}

void GlesContext::color( const Color &c )
{ 
    mColor = c;
    mColorDirty = true;
    updateUniforms();
}

void GlesContext::color( const ColorA &c ) 
{ 
    mColor = c;
    mColorDirty = true;
    updateUniforms();
}

void GlesContext::updateUniforms()
{
    if (!mBound)
        return;

    if (mProjDirty)
        mProg.uniform("uProjection", mProj);

    if (mModelViewDirty)
        mProg.uniform("uModelView",  mModelView);

    if (mColorDirty)
        mProg.uniform("uVertexColor", mColor);

    if (mActiveAttrsDirty) {
        mProg.uniform("uHasVertexAttr",   bool(mActiveAttrs & ES2_ATTR_VERTEX));
        mProg.uniform("uHasTexCoordAttr", bool(mActiveAttrs & ES2_ATTR_TEXCOORD));
        mProg.uniform("uHasColorAttr",    bool(mActiveAttrs & ES2_ATTR_COLOR));
        mProg.uniform("uHasNormalAttr",   bool(mActiveAttrs & ES2_ATTR_NORMAL));
    }

    mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = false;
}

GlesContextRef setGlesContext(GlesContextRef context)
{
    if (context) {
        sContext = context;
    }
    else {
        //  Use default context
        sContext = GlesContext::create();
    }

    return sContext;
}

GlesContextRef getGlesContext()
{
    return sContext;
}

void releaseGlesContext()
{
    if (sContext) {
        sContext = GlesContextRef();
    }
}

#endif // defined( CINDER_GLES2 )

