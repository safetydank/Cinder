#pragma once

#include "cinder/Color.h"

namespace cinder { namespace gl { namespace context {

#if ! defined( CINDER_GLES2 )

//  OpenGL / GLES1 implementation
inline void initialize() {}
inline void bind()       {}
inline void unbind()     {}

void enableClientState(GLenum cap) { glEnableClientState(cap); }

void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glVertexPointer(size, type, stride, pointer);
}
void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glTexCoordPointer(size, type, stride, pointer);
}
void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glColorPointer(size, type, stride, pointer);
}
void normalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glNormalPointer(size, type, stride, pointer);
}

inline void color( float r, float g, float b ) { glColor4f( r, g, b, 1.0f ); }
inline void color( float r, float g, float b, float a ) { glColor4f( r, g, b, a ); }
inline void color( const Color8u &c ) { glColor4ub( c.r, c.g, c.b, 255 ); }
inline void color( const ColorA8u &c ) { glColor4ub( c.r, c.g, c.b, c.a ); }
inline void color( const Color &c ) { glColor4f( c.r, c.g, c.b, 1.0f ); }
inline void color( const ColorA &c ) { glColor4f( c.r, c.g, c.b, c.a ); }

inline void translate( const Vec2f &pos ) { glTranslatef( pos.x, pos.y, 0 ); }
inline void translate( const Vec3f &pos ) { glTranslatef( pos.x, pos.y, pos.z ); }

inline void scale( const Vec3f &scl ) { glScalef( scl.x, scl.y, scl.z ); }

void rotate( const Vec3f &xyz );
void rotate( const Quatf &quat );

void drawElements();
void drawArrays();
void drawRangeElements();

#endif

#if defined( CINDER_GLES2 )

//  Compatibility constants
#if ! defined(GL_VERTEX_ARRAY)
   #define GL_VERTEX_ARRAY                   0x8074
   #define GL_NORMAL_ARRAY                   0x8075
   #define GL_COLOR_ARRAY                    0x8076
   #define GL_TEXTURE_COORD_ARRAY            0x8078
#endif

void initialize();
void bind();
void unbind();

void enableClientState(GLenum cap);

void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void normalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);

void color( float r, float g, float b );
void color( float r, float g, float b, float a );
void color( const Color8u &c );
void color( const ColorA8u &c );
void color( const Color &c );
void color( const ColorA &c );

void translate( const Vec2f &pos );
void translate( const Vec3f &pos );

void scale( const Vec3f &scl );

void rotate( const Vec3f &xyz );
void rotate( const Quatf &quat );

#endif

} } }

