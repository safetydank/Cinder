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

    static void enableClientState(GLenum cap);
    static void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
    static void normalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
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

protected:
    static ES2Context* sContext;
};

typedef ES2Context Context;
#endif

} }

