#pragma once

#include "cinder/Color.h"

namespace cinder { namespace gl {

class ES2Context;
typedef std::shared_ptr<ES2Context> ES2ContextRef;

class GlslProg;

//  GLES2 implementation of a GL context
class ES2Context
{
public:
    static void initialize();
    static void release();
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

} }  //  namespace cinder::gl

// /*  GLES2 equivalents to some of Cinder's GL helpers */
// 
// #include "cinder/Cinder.h"
// #include "cinder/Vector.h"
// #include "cinder/Color.h"
// #include "cinder/Rect.h"
// #include "cinder/PolyLine.h"
// #include "cinder/AxisAlignedBox.h"
// #include "cinder/Color.h"
// #include "cinder/gl/Texture.h"
// #include "cinder/gl/GlslProg.h"
// #include "cinder/Matrix44.h"
// 
// #if defined( CINDER_COCOA_TOUCH )
// 	#include <OpenGLES/ES2/gl.h>
// 	#include <OpenGLES/ES2/glext.h>
// #elif defined( CINDER_ANDROID )
// 	#define GL_GLEXT_PROTOTYPES
// 	#include <GLES2/gl2.h>
// 	#include <GLES2/gl2ext.h>
// #endif
// 
// //  Compatibility constants
// #if ! defined(GL_VERTEX_ARRAY)
// #define GL_VERTEX_ARRAY                   0x8074
// #define GL_NORMAL_ARRAY                   0x8075
// #define GL_COLOR_ARRAY                    0x8076
// #define GL_TEXTURE_COORD_ARRAY            0x8078
// #endif
// 
// // forward declarations
// namespace cinder {
//     class Camera; class TriMesh; class Sphere; 
//     class Path2d; class Shape2d;
//     namespace gl {
//         class VboMesh; class Texture; class GlslProg;
//     }
// } // namespace cinder
// 
// namespace cinder { namespace gl {
// 
// class GLES2Context;
// typedef std::shared_ptr<GLES2Context> GLES2ContextRef;
// 
// //  Should be called by RendererGl
// void initGLES2();
// void freeGLES2();
// 
// void bindGLES2();
// void unbindGLES2();
// 
// class GLES2Context
// {
// public:
//     static void initialize();
// 
// protected:
//     GLES2Context();
//     GLES2Context( GlslProg& shader, GlesAttr& attr );
// 
//     void init( GlslProg& shader, GlesAttr& attr );
// 
// public:
//     //!  Bind GLES2 shader
//     void bind();
// 
//     //!  Unbind GLES2 shader
//     void unbind();
// 
//     virtual void selectAttrs(uint32_t activeAttrs);
// 
//     GlesAttr& attr();
// 
//     void setMatrices( const Camera &cam );
//     void setModelView( const Camera &cam );
//     void setProjection( const Camera &cam );
//     void setProjection( const Matrix44f &proj );
//     void pushModelView();
//     void popModelView();
//     void pushModelView( const Camera &cam );
//     void pushProjection( const Camera &cam );
//     void pushMatrices();
//     void popMatrices();
//     void multModelView( const Matrix44f &mtx );
//     void multProjection( const Matrix44f &mtx );
// 
//     Matrix44f getModelView();
//     Matrix44f getProjection();
// 
//     void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
//     inline void setMatricesWindowPersp( const Vec2i &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
//     { setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
//     void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
//     inline void setMatricesWindow( const Vec2i &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }
// 
//     //  assumes we'll always be working on the modelview matrix, otherwise we can add
//     //  matrix mode tracking later...
//     void        translate( const Vec2f &pos );
//     inline void translate( float x, float y ) { translate( Vec2f( x, y ) ); }
//     void        translate( const Vec3f &pos );
//     inline void translate( float x, float y, float z ) { translate( Vec3f( x, y, z ) ); }
// 
//     void        scale( const Vec3f &scl );
//     inline void scale( const Vec2f &scl ) { scale( Vec3f( scl.x, scl.y, 1.0f ) ); }
//     inline void scale( float x, float y ) { scale( Vec3f( x, y, 1.0f ) ); }
//     inline void scale( float x, float y, float z ) { scale( Vec3f( x, y, z ) ); }
// 
//     void        rotate( const Vec3f &xyz );
//     void        rotate( const Quatf &quat );
//     inline void rotate( float degrees ) { rotate( Vec3f( 0, 0, degrees ) ); }
// 
//     void color( float r, float g, float b );
//     void color( float r, float g, float b, float a );
//     void color( const Color8u &c );
//     void color( const ColorA8u &c );
//     void color( const Color &c );
//     void color( const ColorA &c );
// 
// protected:
//     void updateUniforms();
// 
//     //  Shader program
//     GlslProg mProg;
// 
//     //  Shader attributes
//     GlesAttr mAttr;
// 
//     //  Uniforms
//     Matrix44f mProj;
//     Matrix44f mModelView;
//     ColorA    mColor;
//     Texture   mTexture;
//     uint32_t  mActiveAttrs;
// 
//     bool mBound;
// 
//     bool mProjDirty;
//     bool mModelViewDirty;
//     bool mColorDirty;
//     bool mTextureDirty;
//     bool mActiveAttrsDirty;
// 
//     std::vector<Matrix44f> mModelViewStack;
//     std::vector<Matrix44f> mProjStack;
// };
// 
// 
// //  Attributes used by the draw* methods
// enum ShaderAttrs
// {
//     ES2_ATTR_NONE     = 0,
//     ES2_ATTR_VERTEX   = 1 << 0,
//     ES2_ATTR_TEXCOORD = 1 << 1,
//     ES2_ATTR_COLOR    = 1 << 2,
//     ES2_ATTR_NORMAL   = 1 << 3,
// };
// 
// class SelectAttrCallback;
// 
// //  Shader attributes and draw method implementations
// struct GlesAttr 
// {
//     GLuint mVertex;
//     GLuint mTexCoord;
//     GLuint mColor;
//     GLuint mNormal;
//     GLuint mTexSampler;
// 
//     SelectAttrCallback* mSelectAttr;
// 
//     GlesAttr(GLuint vertex=0, GLuint texCoord=0, GLuint color=0, GLuint normal=0);
//     inline void selectAttrs(uint32_t activeAttrs) { if ( mSelectAttr ) mSelectAttr->selectAttrs( activeAttrs ); }
// 
//     void drawLine( const Vec2f &start, const Vec2f &end );
//     void drawLine( const Vec3f &start, const Vec3f &end );
//     void drawCube( const Vec3f &center, const Vec3f &size );
//     void drawColorCube( const Vec3f &center, const Vec3f &size );
//     void drawStrokedCube( const Vec3f &center, const Vec3f &size );
//     inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
//     void drawSphere( const Vec3f &center, float radius, int segments = 12 );
//     void draw( const class Sphere &sphere, int segments = 12 );
//     void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
//     void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
//     void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
//     void drawStrokedRect( const Rectf &rect );
//     void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//     void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
//     void drawFrustum( const Camera &cam );
//     void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
//     void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
//     // void draw( const class PolyLine<Vec2f> &polyLine );
//     void draw( const class PolyLine<Vec3f> &polyLine );
//     void draw( const class Path2d &path2d, float approximationScale = 1.0f );
//     void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );
// 
//     void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );
// 
//     void draw( const TriMesh &mesh );
//     void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
//     void draw( const VboMesh &vbo );
//     void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
//     void drawArrays( const VboMesh &vbo, GLint first, GLsizei count );
//     void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
//     void draw( const Texture &texture );
//     void draw( const Texture &texture, const Vec2f &pos );
//     void draw( const Texture &texture, const Rectf &rect );
//     void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect );
// };
// 
// //! Convenience class designed to push and pop a boolean OpenGL state
// //  Only supports checking the enabled state of vertex arrays supported by
// //  GlesAttr, ie GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY,
// //  GL_NORMAL_ARRAY
// struct ClientBoolState {
// 	ClientBoolState( GLint target );
// 	ClientBoolState( GlesAttr& attr, GLint target );
// 	~ClientBoolState();
//   private:
//     void init(GlesAttr& attr, GLint target );
// 	GLuint		mTarget;
// 	int         mOldValue;
// };
// 
// //! Convenience class designed to push and pop the current color
// struct SaveColorState {
// 	SaveColorState();
// 	~SaveColorState();
//   private:
// 	GLfloat		mOldValues[4];
// };
// 
// } }

