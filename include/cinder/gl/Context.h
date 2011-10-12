#pragma once

#if defined( CINDER_GLES2 )
  #include "cinder/gles2.h"
#endif

namespace cinder { namespace gl {

#if ! defined( CINDER_GLES2 )

class GLContext
{
    static void initialize();
};

#endif

#if defined( CINDER_GLES2 )

class GLES2Context;
typedef std::shared_ptr<GLES2Context> GLES2ContextRef;

//  Should be called by RendererGl
void initGLES2();
void freeGLES2();

void bindGLES2();
void unbindGLES2();

class GLES2Context : public SelectAttrCallback
{
public:
    static void initialize();

protected:
    GLES2Context();
    GLES2Context( GlslProg& shader, GlesAttr& attr );

    void init( GlslProg& shader, GlesAttr& attr );

public:
    //!  Bind GLES2 shader
    void bind();

    //!  Unbind GLES2 shader
    void unbind();

    virtual void selectAttrs(uint32_t activeAttrs);

    GlesAttr& attr();

    void setMatrices( const Camera &cam );
    void setModelView( const Camera &cam );
    void setProjection( const Camera &cam );
    void setProjection( const Matrix44f &proj );
    void pushModelView();
    void popModelView();
    void pushModelView( const Camera &cam );
    void pushProjection( const Camera &cam );
    void pushMatrices();
    void popMatrices();
    void multModelView( const Matrix44f &mtx );
    void multProjection( const Matrix44f &mtx );

    Matrix44f getModelView();
    Matrix44f getProjection();

    void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
    inline void setMatricesWindowPersp( const Vec2i &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
    { setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
    void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
    inline void setMatricesWindow( const Vec2i &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }

    //  assumes we'll always be working on the modelview matrix, otherwise we can add
    //  matrix mode tracking later...
    void        translate( const Vec2f &pos );
    inline void translate( float x, float y ) { translate( Vec2f( x, y ) ); }
    void        translate( const Vec3f &pos );
    inline void translate( float x, float y, float z ) { translate( Vec3f( x, y, z ) ); }

    void        scale( const Vec3f &scl );
    inline void scale( const Vec2f &scl ) { scale( Vec3f( scl.x, scl.y, 1.0f ) ); }
    inline void scale( float x, float y ) { scale( Vec3f( x, y, 1.0f ) ); }
    inline void scale( float x, float y, float z ) { scale( Vec3f( x, y, z ) ); }

    void        rotate( const Vec3f &xyz );
    void        rotate( const Quatf &quat );
    inline void rotate( float degrees ) { rotate( Vec3f( 0, 0, degrees ) ); }

    void color( float r, float g, float b );
    void color( float r, float g, float b, float a );
    void color( const Color8u &c );
    void color( const ColorA8u &c );
    void color( const Color &c );
    void color( const ColorA &c );

protected:
    void updateUniforms();

    //  Shader program
    GlslProg mProg;

    //  Shader attributes
    GlesAttr mAttr;

    //  Uniforms
    Matrix44f mProj;
    Matrix44f mModelView;
    ColorA    mColor;
    Texture   mTexture;
    uint32_t  mActiveAttrs;

    bool mBound;

    bool mProjDirty;
    bool mModelViewDirty;
    bool mColorDirty;
    bool mTextureDirty;
    bool mActiveAttrsDirty;

    std::vector<Matrix44f> mModelViewStack;
    std::vector<Matrix44f> mProjStack;
};

typedef GLES2Context GLContext;

#endif

} }

