#pragma once

#include "cinder/Color.h"

namespace cinder { namespace gl {

#if ! defined( CINDER_GLES2 )

//  OpenGL / GLES1 implementation
class GLContext
{
public:
    static void initialize() {}
    static void bind()       {}
    static void unbind()     {}

    static void color( float r, float g, float b ) { glColor4f( r, g, b, 1.0f ); }
    static void color( float r, float g, float b, float a ) { glColor4f( r, g, b, a ); }
    static void color( const Color8u &c ) { glColor4ub( c.r, c.g, c.b, 255 ); }
    static void color( const ColorA8u &c ) { glColor4ub( c.r, c.g, c.b, c.a ); }
    static void color( const Color &c ) { glColor4f( c.r, c.g, c.b, 1.0f ); }
    static void color( const ColorA &c ) { glColor4f( c.r, c.g, c.b, c.a ); }

    static void enableClientState(GLenum cap) { glEnableClientState(cap); }
    static void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
        glVertexPointer(size, type, stride, pointer);
    }
    static void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
        glTexCoordPointer(size, type, stride, pointer);
    }
    static void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
        glColorPointer(size, type, stride, pointer);
    }
    static void normalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
        glNormalPointer(size, type, stride, pointer);
    }

    static void setModelView( const Camera &cam );
    static void setProjection( const Camera &cam );
    static void setMatrices( const Camera &cam );
    static void pushModelView();
    static void popModelView();
    static void pushModelView( const Camera &cam );
    static void pushProjection( const Camera &cam );
    static void pushMatrices();
    static void popMatrices();
    static void multModelView( const Matrix44f &mtx );
    static void multProjection( const Matrix44f &mtx );
    static Matrix44f getModelView();
    static Matrix44f getProjection();
    static void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft );
    static void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft );

    static void translate( const Vec2f &pos );
    static void translate( const Vec3f &pos );
    static void scale( const Vec3f &scl );
    static void rotate( const Vec3f &xyz );
    static void rotate( const Quatf &quat );
};

typedef GLContext Context;
#endif

#if defined( CINDER_GLES2 )

//  Compatibility constants
#if ! defined(GL_VERTEX_ARRAY)
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_TEXTURE_COORD_ARRAY            0x8078
#endif

class ES2Context;
typedef std::shared_ptr<ES2Context> ES2ContextRef;

class GlslProg;

//  GLES2 implementation
class ES2Context
{
public:
    static void initialize();
    static void bind();
    static void unbind();

    static void color( float r, float g, float b );
    static void color( float r, float g, float b, float a );
    static void color( const Color8u &c );
    static void color( const ColorA8u &c );
    static void color( const Color &c );
    static void color( const ColorA &c );

    static void enableClientState(GLenum cap);
    static void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void normalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);

    static void translate( const Vec2f &pos );
    static void translate( const Vec3f &pos );
    static void scale( const Vec3f &scl );
    static void rotate( const Vec3f &xyz );
    static void rotate( const Quatf &quat );

protected:
    static ES2ContextRef   sContext;
    GlslProg&              mShader;

    std::vector<Matrix44f> mModelViewStack;
    std::vector<Matrix44f> mProjStack;

    Matrix44f mProj;
    Matrix44f mModelView;
    ColorA    mColor;
    Texture   mTexture;
    uint32_t  mActiveAttrs;

    bool      mModelViewDirty;

    ES2Context();
    ~ES2Context();

    void updateUniforms();
};

typedef ES2Context Context;
#endif

} }

