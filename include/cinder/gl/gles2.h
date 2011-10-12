#pragma once

/*  GLES2 equivalents to some of Cinder's GL helpers */

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/PolyLine.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Matrix44.h"

#if defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#elif defined( CINDER_ANDROID )
	#define GL_GLEXT_PROTOTYPES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

//  Compatibility constants
#if ! defined(GL_VERTEX_ARRAY)
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_TEXTURE_COORD_ARRAY            0x8078
#endif

// forward declarations
namespace cinder {
    class Camera; class TriMesh; class Sphere; 
    class Path2d; class Shape2d;
    namespace gl {
        class VboMesh; class Texture; class GlslProg;
    }
} // namespace cinder

namespace cinder { namespace gl {

//  Attributes used by the draw* methods
enum ShaderAttrs
{
    ES2_ATTR_NONE     = 0,
    ES2_ATTR_VERTEX   = 1 << 0,
    ES2_ATTR_TEXCOORD = 1 << 1,
    ES2_ATTR_COLOR    = 1 << 2,
    ES2_ATTR_NORMAL   = 1 << 3,
};

class SelectAttrCallback;

//  Shader attributes and draw method implementations
struct GlesAttr 
{
    GLuint mVertex;
    GLuint mTexCoord;
    GLuint mColor;
    GLuint mNormal;
    GLuint mTexSampler;

    SelectAttrCallback* mSelectAttr;

    GlesAttr(GLuint vertex=0, GLuint texCoord=0, GLuint color=0, GLuint normal=0);
    inline void selectAttrs(uint32_t activeAttrs) { if ( mSelectAttr ) mSelectAttr->selectAttrs( activeAttrs ); }

    void drawLine( const Vec2f &start, const Vec2f &end );
    void drawLine( const Vec3f &start, const Vec3f &end );
    void drawCube( const Vec3f &center, const Vec3f &size );
    void drawColorCube( const Vec3f &center, const Vec3f &size );
    void drawStrokedCube( const Vec3f &center, const Vec3f &size );
    inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
    void drawSphere( const Vec3f &center, float radius, int segments = 12 );
    void draw( const class Sphere &sphere, int segments = 12 );
    void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
    void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
    void drawStrokedRect( const Rectf &rect );
    void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
    void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
    void drawFrustum( const Camera &cam );
    void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
    void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
    // void draw( const class PolyLine<Vec2f> &polyLine );
    void draw( const class PolyLine<Vec3f> &polyLine );
    void draw( const class Path2d &path2d, float approximationScale = 1.0f );
    void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

    void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );

    void draw( const TriMesh &mesh );
    void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
    void draw( const VboMesh &vbo );
    void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
    void drawArrays( const VboMesh &vbo, GLint first, GLsizei count );
    void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
    void draw( const Texture &texture );
    void draw( const Texture &texture, const Vec2f &pos );
    void draw( const Texture &texture, const Rectf &rect );
    void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect );
};

//! Convenience class designed to push and pop a boolean OpenGL state
//  Only supports checking the enabled state of vertex arrays supported by
//  GlesAttr, ie GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY,
//  GL_NORMAL_ARRAY
struct ClientBoolState {
	ClientBoolState( GLint target );
	ClientBoolState( GlesAttr& attr, GLint target );
	~ClientBoolState();
  private:
    void init(GlesAttr& attr, GLint target );
	GLuint		mTarget;
	int         mOldValue;
};

//! Convenience class designed to push and pop the current color
struct SaveColorState {
	SaveColorState();
	~SaveColorState();
  private:
	GLfloat		mOldValues[4];
};

} }

