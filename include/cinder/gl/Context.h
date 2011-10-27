#pragma once

#include "cinder/Color.h"

namespace cinder { namespace gl { namespace context {

//  OpenGL / GLES1 implementation
#if ! defined( CINDER_GLES2 )

inline void initialize() {}
inline void bind()       {}
inline void unbind()     {}

inline void enableClientState(GLenum cap) { glEnableClientState(cap); }
inline void disableClientState(GLenum cap) { glDisableClientState(cap); }

inline void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glVertexPointer(size, type, stride, pointer);
}
inline void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glTexCoordPointer(size, type, stride, pointer);
}
inline void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) {
    glColorPointer(size, type, stride, pointer);
}
inline void normalPointer(GLenum type, GLsizei stride, const GLvoid* pointer) {
    glNormalPointer(type, stride, pointer);
}

inline void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    glDrawElements(mode, count, type, indices);
}

inline void drawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
}

#if ! defined( CINDER_GLES1 )
inline void drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
    glDrawRangeElements(mode, start, end, count, type, indices);
}
#endif


inline void color( float r, float g, float b ) { glColor4f( r, g, b, 1.0f ); }
inline void color( float r, float g, float b, float a ) { glColor4f( r, g, b, a ); }
inline void color( const Color8u &c ) { glColor4ub( c.r, c.g, c.b, 255 ); }
inline void color( const ColorA8u &c ) { glColor4ub( c.r, c.g, c.b, c.a ); }
inline void color( const Color &c ) { glColor4f( c.r, c.g, c.b, 1.0f ); }
inline void color( const ColorA &c ) { glColor4f( c.r, c.g, c.b, c.a ); }

#endif

//  OpenGL ES2 implementation
#if defined( CINDER_GLES2 )

//  Compatibility constants
#if ! defined( GL_VERTEX_ARRAY )
   #define GL_VERTEX_ARRAY                   0x8074
   #define GL_NORMAL_ARRAY                   0x8075
   #define GL_COLOR_ARRAY                    0x8076
   #define GL_TEXTURE_COORD_ARRAY            0x8078
#endif

void initialize();
void bind();
void unbind();

void enableClientState(GLenum cap);
void disableClientState(GLenum cap);

void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void normalPointer(GLenum type, GLsizei stride, const GLvoid* pointer);

void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
void drawArrays(GLenum mode, GLint first, GLsizei count);
//! GLES2 implementation of drawRangeElements ignores the start and end arguments
void drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

void color( float r, float g, float b );
void color( float r, float g, float b, float a );
void color( const Color8u &c );
void color( const ColorA8u &c );
void color( const Color &c );
void color( const ColorA &c );

#endif

} } }

