#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"

namespace cinder { namespace gl { namespace context {

#define GLES_CONTEXT_METHOD(FN, ...) assert(ES2Context::sContext); ES2Context::sContext->FN(__VA_ARGS__);

class ES2Context;
typedef std::shared_ptr<ES2Context> ES2ContextRef;

class ES2Context
{
public:
    //  Singleton
    static ES2ContextRef sContext;

    ~ES2Context()
    {
    }

    void initialize()
    {
        sContext = ES2ContextRef();
        sContext = ES2ContextRef(new ES2Context());
    }

    void bind()
    {
    }

    void unbind()
    {
    }

    void enableClientState(GLenum cap)
    {
    }

protected:
    ES2Context()
    {
    }
};

void initialize() 
{ 
    GLES_CONTEXT_METHOD(initialize) 
}

void bind()
{
    GLES_CONTEXT_METHOD(bind) 
}

void unbind()
{
    GLES_CONTEXT_METHOD(unbind)
}

void enableClientState(GLenum cap)
{
    GLES_CONTEXT_METHOD(enableClientState, cap)
}

} } }


// #include "cinder/gl/Context.h"
// #include "cinder/gl/gles2.h"
// 
// #include "cinder/gl/GlslProg.h"
// #include "cinder/gl/Vbo.h"
// #include "cinder/CinderMath.h"
// #include "cinder/Vector.h"
// #include "cinder/Camera.h"
// #include "cinder/TriMesh.h"
// #include "cinder/Sphere.h"
// #include "cinder/gl/Texture.h"
// #include "cinder/Text.h"
// #include "cinder/PolyLine.h"
// #include "cinder/Path2d.h"
// #include "cinder/Shape2d.h"
// 
// using std::shared_ptr;
// 
// namespace cinder { namespace gl {
// 
// //  Check if a texture is bound - called before enabling texturing in the
// //  shader program
// inline ShaderAttrs useTexCoordFlag()
// {
//     int texID;
//     glGetIntegerv( GL_TEXTURE_BINDING_2D, &texID );
//     return ( texID == 0 ? ES2_ATTR_NONE : ES2_ATTR_TEXCOORD );
// }
// 
// GlesAttr::GlesAttr(GLuint vertex, GLuint texCoord, GLuint color, GLuint normal)
//    : mVertex(vertex), mTexCoord(texCoord), mColor(color), mNormal(normal), mSelectAttr(NULL)
// { }
// 
// void GlesAttr::drawLine( const Vec2f &start, const Vec2f &end )
// {
//     drawLine(Vec3f(start.x, start.y, 0), Vec3f(end.x, end.y, 0));
// }
// 
// void GlesAttr::drawLine( const Vec3f &start, const Vec3f &end )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
//     float lineVerts[3*2];
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, lineVerts );
//     lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
//     lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
//     glDrawArrays( GL_LINES, 0, 2 );
//     glDisableVertexAttribArray(mVertex);
// }
// 
// namespace {
// 
// void drawCubeImpl( GlesAttr& attr, const Vec3f &c, const Vec3f &size, bool drawColors )
// {
//     attr.selectAttrs( ES2_ATTR_VERTEX 
//                     | useTexCoordFlag() 
//                     | (drawColors ? ES2_ATTR_COLOR : 0)
//                     | ES2_ATTR_NORMAL );
// 
// 	GLfloat sx = size.x * 0.5f;
// 	GLfloat sy = size.y * 0.5f;
// 	GLfloat sz = size.z * 0.5f;
// 	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
// 							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
// 							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
// 							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
// 							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
// 							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z
// 
// 
// 	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
// 								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
// 									0,0,1,	0,0,1,	0,0,1,	0,0,1,
// 								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
// 								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
// 								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};
// 	 
// 	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
// 									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
// 									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
// 									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
// 									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
// 									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow
// 
// 	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
// 								1,1,	1,0,	0,0,	0,1,
// 								0,1,	1,1,	1,0,	0,0,							
// 								1,1,	1,0,	0,0,	0,1,
// 								1,0,	0,0,	0,1,	1,1,
// 								1,0,	0,0,	0,1,	1,1 };
// 	
// 	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
// 									4, 5, 6, 4, 6, 7,
// 									8, 9,10, 8, 10,11,
// 									12,13,14,12,14,15,
// 									16,17,18,16,18,19,
// 									20,21,22,20,22,23 };
// 
//     glEnableVertexAttribArray(attr.mNormal);
//     glVertexAttribPointer( attr.mNormal, 3, GL_FLOAT, GL_FALSE, 0, normals );
// 
//     glEnableVertexAttribArray(attr.mTexCoord);
//     glVertexAttribPointer( attr.mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texs );
// 
// 	if( drawColors ) {
//         glEnableVertexAttribArray(attr.mColor);
//         glVertexAttribPointer( attr.mColor, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, colors );
// 	}
// 
//     glEnableVertexAttribArray(attr.mVertex);
//     glVertexAttribPointer( attr.mVertex, 3, GL_FLOAT, GL_FALSE, 0, vertices );
// 
// 	glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );
// 
// 	glDisableVertexAttribArray( attr.mVertex );
// 	glDisableVertexAttribArray( attr.mTexCoord );	 
// 	glDisableVertexAttribArray( attr.mNormal );
// 	if( drawColors )
// 		glDisableVertexAttribArray( attr.mColor );
// 	 
// }
// } // anonymous namespace
// 
// void GlesAttr::drawCube( const Vec3f &center, const Vec3f &size )
// {
// 	drawCubeImpl( *this, center, size, false );
// }
// 
// 
// void GlesAttr::drawColorCube( const Vec3f &center, const Vec3f &size )
// {
// 	drawCubeImpl( *this, center, size, true );
// }
// 
// 
// void GlesAttr::drawStrokedCube( const Vec3f &center, const Vec3f &size )
// {
// 
// 	Vec3f min = center - size * 0.5f;
// 	Vec3f max = center + size * 0.5f;
// 
// 	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z) );
// 	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z) );
// 	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z) );
// 	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z) );
// 	
// 	drawLine( Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z) );
// 	drawLine( Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z) );
// 	drawLine( Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z) );
// 	drawLine( Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z) );
// 	
// 	drawLine( Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z) );
// 	drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z) );
// 	drawLine( Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z) );
// 	drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z) );
// }
// 
// 
// void GlesAttr::drawSphere( const Vec3f &center, float radius, int segments )
// {
//     selectAttrs( ES2_ATTR_VERTEX | useTexCoordFlag() | ES2_ATTR_NORMAL );
// 
// 	if( segments < 0 )
// 		return;
// 
// 	float *verts = new float[(segments+1)*2*3];
// 	float *normals = new float[(segments+1)*2*3];
// 	float *texCoords = new float[(segments+1)*2*2];
// 
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
//     glEnableVertexAttribArray(mTexCoord);
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
//     glEnableVertexAttribArray(mNormal);
//     glVertexAttribPointer( mNormal, 3, GL_FLOAT, GL_FALSE, 0, normals );
// 
// 	for( int j = 0; j < segments / 2; j++ ) {
// 		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
// 		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
// 
// 		for( int i = 0; i <= segments; i++ ) {
// 			Vec3f e, p;
// 			float theta3 = i * 2 * 3.14159f / segments;
// 
// 			e.x = math<float>::cos( theta1 ) * math<float>::cos( theta3 );
// 			e.y = math<float>::sin( theta1 );
// 			e.z = math<float>::cos( theta1 ) * math<float>::sin( theta3 );
// 			p = e * radius + center;
// 			normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
// 			texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
// 			verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;
// 
// 			e.x = math<float>::cos( theta2 ) * math<float>::cos( theta3 );
// 			e.y = math<float>::sin( theta2 );
// 			e.z = math<float>::cos( theta2 ) * math<float>::sin( theta3 );
// 			p = e * radius + center;
// 			normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
// 			texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
// 			verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
// 		}
// 		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
// 	}
// 
//     glDisableVertexAttribArray( mVertex );
//     glDisableVertexAttribArray( mTexCoord );
//     glDisableVertexAttribArray( mNormal );
// 	
// 	delete [] verts;
// 	delete [] normals;
// 	delete [] texCoords;
// }
// 
// 
// void GlesAttr::draw( const class Sphere &sphere, int segments )
// {
// 	drawSphere( sphere.getCenter(), sphere.getRadius(), segments );
// }
// 
// 
// void GlesAttr::drawSolidCircle( const Vec2f &center, float radius, int numSegments )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
// 	// automatically determine the number of segments from the circumference
// 	if( numSegments <= 0 ) {
// 		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
// 	}
// 	if( numSegments < 2 ) numSegments = 2;
// 	
// 	GLfloat *verts = new float[(numSegments+2)*3];
// 	verts[0] = center.x;
// 	verts[1] = center.y;
//     verts[2] = 0;
// 	for( int s = 0; s <= numSegments; s++ ) {
// 		float t = s / (float)numSegments * 2.0f * 3.14159f;
// 		verts[(s+1)*3+0] = center.x + math<float>::cos( t ) * radius;
// 		verts[(s+1)*3+1] = center.y + math<float>::sin( t ) * radius;
// 		verts[(s+1)*3+2] = 0;
// 	}
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
// 	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
//     glDisableVertexAttribArray(mVertex);
// 	delete [] verts;
// }
// 
// 
// void GlesAttr::drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
// 	// automatically determine the number of segments from the circumference
// 	if( numSegments <= 0 ) {
// 		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
// 	}
// 	if( numSegments < 2 ) numSegments = 2;
// 	
// 	GLfloat *verts = new float[numSegments*3];
// 	for( int s = 0; s < numSegments; s++ ) {
// 		float t = s / (float)numSegments * 2.0f * 3.14159f;
// 		verts[s*3+0] = center.x + math<float>::cos( t ) * radius;
// 		verts[s*3+1] = center.y + math<float>::sin( t ) * radius;
// 		verts[s*3+2] = 0;
// 	}
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
// 	glDrawArrays( GL_LINE_LOOP, 0, numSegments );
//     glDisableVertexAttribArray(mVertex);
// 	delete [] verts;
// }
// 
// 
// void GlesAttr::drawSolidRect( const Rectf &rect, bool textureRectangle )
// {
//     selectAttrs( ES2_ATTR_VERTEX | ES2_ATTR_TEXCOORD );
// 
//     glEnableVertexAttribArray(mVertex);
// 	GLfloat verts[12];
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
//     glEnableVertexAttribArray(mTexCoord);
// 	GLfloat texCoords[8];
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
// 	verts[0*3+0] = rect.getX2(); texCoords[0*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
// 	verts[0*3+1] = rect.getY1(); texCoords[0*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
//     verts[0*3+2] = 0;
// 	verts[1*3+0] = rect.getX1(); texCoords[1*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
// 	verts[1*3+1] = rect.getY1(); texCoords[1*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
//     verts[1*3+2] = 0;
// 	verts[2*3+0] = rect.getX2(); texCoords[2*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
// 	verts[2*3+1] = rect.getY2(); texCoords[2*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
//     verts[2*3+2] = 0;
// 	verts[3*3+0] = rect.getX1(); texCoords[3*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
// 	verts[3*3+1] = rect.getY2(); texCoords[3*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
//     verts[3*3+2] = 0;
// 
// 	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
// 
//     glDisableVertexAttribArray(mVertex);
//     glDisableVertexAttribArray(mTexCoord);	 
// }
// 
// 
// void GlesAttr::drawStrokedRect( const Rectf &rect )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 	GLfloat verts[12];
// 	verts[ 0] = rect.getX1();	verts[ 1] = rect.getY1();    verts[ 2] = 0;
// 	verts[ 3] = rect.getX2();	verts[ 4] = rect.getY1();    verts[ 5] = 0;
// 	verts[ 6] = rect.getX2();	verts[ 7] = rect.getY2();    verts[ 8] = 0;
// 	verts[ 9] = rect.getX1();	verts[10] = rect.getY2();    verts[11] = 0;
//     glEnableVertexAttribArray( mVertex );
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
// 	glDrawArrays( GL_LINE_LOOP, 0, 4 );
//     glDisableVertexAttribArray(mVertex);
// }
// 
// 
// void GlesAttr::drawCoordinateFrame( float axisLength, float headLength, float headRadius )
// {
// 	// XXX glColor4ub( 255, 0, 0, 255 );
// 	drawVector( Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
// 	// XXX glColor4ub( 0, 255, 0, 255 );
// 	drawVector( Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
// 	// XXX glColor4ub( 0, 0, 255, 255 );
// 	drawVector( Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
// }
// 
// 
// void GlesAttr::drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
// 	const int NUM_SEGMENTS = 32;
// 	float lineVerts[3*2];
// 	Vec3f coneVerts[NUM_SEGMENTS+2];
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, lineVerts );
// 	lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
// 	lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z;	
// 	glDrawArrays( GL_LINES, 0, 2 );
// 	
// 	// Draw the cone
// 	Vec3f axis = ( end - start ).normalized();
// 	Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
// 	Vec3f left = axis.cross( temp ).normalized();
// 	Vec3f up = axis.cross( left ).normalized();
// 
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &coneVerts[0].x );
// 	coneVerts[0] = Vec3f( end + axis * headLength );
// 	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
// 		float t = s / (float)NUM_SEGMENTS;
// 		coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
// 			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
// 	}
// 	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );
// 
// 	// draw the cap
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &coneVerts[0].x );
// 	coneVerts[0] = end;
// 	for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
// 		float t = s / (float)NUM_SEGMENTS;
// 		coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
// 			+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
// 	}
// 	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );
// 
//     glDisableVertexAttribArray(mVertex);
// }
// 
// 
// void GlesAttr::drawFrustum( const Camera &cam )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
// 	Vec3f vertex[8];
// 	Vec3f nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
// 	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );
// 
// 	Vec3f farTopLeft, farTopRight, farBottomLeft, farBottomRight;
// 	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );
// 	
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &vertex[0].x );
// 	
// 	vertex[0] = cam.getEyePoint();
// 	vertex[1] = nearTopLeft;
// 	vertex[2] = cam.getEyePoint();
// 	vertex[3] = nearTopRight;
// 	vertex[4] = cam.getEyePoint();
// 	vertex[5] = nearBottomRight;
// 	vertex[6] = cam.getEyePoint();
// 	vertex[7] = nearBottomLeft;
// 	glDrawArrays( GL_LINES, 0, 8 );
// 
// 	vertex[0] = farTopLeft;
// 	vertex[1] = nearTopLeft;
// 	vertex[2] = farTopRight;
// 	vertex[3] = nearTopRight;
// 	vertex[4] = farBottomRight;
// 	vertex[5] = nearBottomRight;
// 	vertex[6] = farBottomLeft;
// 	vertex[7] = nearBottomLeft;
// 	glDrawArrays( GL_LINES, 0, 8 );
// 
// 	glLineWidth( 2.0f );
// 	vertex[0] = nearTopLeft;
// 	vertex[1] = nearTopRight;
// 	vertex[2] = nearBottomRight;
// 	vertex[3] = nearBottomLeft;
// 	glDrawArrays( GL_LINE_LOOP, 0, 4 );
// 
// 	vertex[0] = farTopLeft;
// 	vertex[1] = farTopRight;
// 	vertex[2] = farBottomRight;
// 	vertex[3] = farBottomLeft;
// 	glDrawArrays( GL_LINE_LOOP, 0, 4 );
// 	
// 	glLineWidth( 1.0f );
//     glDisableVertexAttribArray(mVertex);
// }
// 
// 
// void GlesAttr::drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
// {
//     selectAttrs( ES2_ATTR_VERTEX | useTexCoordFlag() | ES2_ATTR_NORMAL );
// 
// 	longitudeSegments = std::min( std::max( 7, longitudeSegments ) + 1, 255 );
// 	latitudeSegments = std::min( std::max( 7, latitudeSegments ) + 1, 255 );
// 
// 	int i, j;
// 	float *normal = new float[longitudeSegments * latitudeSegments * 3];
// 	float *vertex = new float[longitudeSegments * latitudeSegments * 3];
// 	float *tex = new float[longitudeSegments * latitudeSegments * 2];
// 	GLushort *indices = new GLushort[latitudeSegments * 2];
// 	float ct, st, cp, sp;
// 
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, vertex );
//     glEnableVertexAttribArray(mTexCoord);
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, tex );
//     glEnableVertexAttribArray(mNormal);
//     glVertexAttribPointer( mNormal, 3, GL_FLOAT, GL_FALSE, 0, normal );
// 
// 	for( i = 0; i < longitudeSegments; i++ ) {
// 		ct = cos(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));
// 		st = sin(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));
// 
// 		for ( j = 0; j < latitudeSegments; j++ ) {
// 			cp = cos(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));
// 			sp = sin(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));
// 
// 			normal[3 * (i + longitudeSegments * j)    ] = cp * ct;
// 			normal[3 * (i + longitudeSegments * j) + 1] = sp * ct;
// 			normal[3 * (i + longitudeSegments * j) + 2] = st;
// 
// 			tex[2 * (i + longitudeSegments * j)    ] = 1.0f * (float)i / (float)(longitudeSegments - 1);
// 			tex[2 * (i + longitudeSegments * j) + 1] = 5.0f * (float)j / (float)(latitudeSegments - 1);
// 
// 			vertex[3 * (i + longitudeSegments * j)    ] = cp * (outterRadius  + innerRadius * ct);
// 			vertex[3 * (i + longitudeSegments * j) + 1] = sp * (outterRadius  + innerRadius * ct);
// 			vertex[3 * (i + longitudeSegments * j) + 2] = innerRadius * st;
// 		}
// 	}
// 
// 	for ( i = 0; i < longitudeSegments - 1; i++ ) {
// 		for ( j = 0; j < latitudeSegments; j++ ) {
// 			indices[j*2+0] = i + 1 + longitudeSegments * j;
// 			indices[j*2+1] = i + longitudeSegments * j;
// 		}
// 		glDrawElements( GL_TRIANGLE_STRIP, (latitudeSegments)*2, GL_UNSIGNED_SHORT, indices );
// 	}
// 
//     glDisableVertexAttribArray( mVertex );
//     glDisableVertexAttribArray( mTexCoord );
//     glDisableVertexAttribArray( mNormal );
// 	
// 	
// 	delete [] normal;
// 	delete [] tex;
// 	delete [] vertex;
// 	delete [] indices;
// }
// 
// 
// void GlesAttr::drawCylinder( float baseRadius, float topRadius, float height, int slices, int stacks )
// {
//     selectAttrs( ES2_ATTR_VERTEX | useTexCoordFlag() | ES2_ATTR_NORMAL );
// 
// 	stacks = math<int>::max(2, stacks + 1);	// minimum of 1 stack
// 	slices = math<int>::max(4, slices + 1);	// minimum of 3 slices
// 
// 	int i, j;
// 	float *normal = new float[stacks * slices * 3];
// 	float *vertex = new float[stacks * slices * 3];
// 	float *tex = new float[stacks * slices * 2];
// 	GLushort *indices = new GLushort[slices * 2];
// 
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, vertex );
//     glEnableVertexAttribArray(mTexCoord);
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, tex );
//     glEnableVertexAttribArray(mNormal);
//     glVertexAttribPointer( mNormal, 3, GL_FLOAT, GL_FALSE, 0, normal );
// 
// 	for(i=0;i<slices;i++) {
// 		float u = (float)i / (float)(slices - 1);
// 		float ct = cos(2.0f * (float)M_PI * u);
// 		float st = sin(2.0f * (float)M_PI * u);
// 
// 		for(j=0;j<stacks;j++) {
// 			float v = (float)j / (float)(stacks-1);
// 			float radius = lerp<float>(baseRadius, topRadius, v); 
// 
// 			int index = 3 * (i * stacks + j);
// 
// 			normal[index    ] = ct;
// 			normal[index + 1] = 0;
// 			normal[index + 2] = st;
// 
// 			tex[2 * (i * stacks + j)    ] = u;
// 			tex[2 * (i * stacks + j) + 1] = 1.0f - v; // top of texture is top of cylinder
// 
// 			vertex[index    ] = ct * radius;
// 			vertex[index + 1] = v * height;
// 			vertex[index + 2] = st * radius;
// 		}
// 	}
// 
// 	for(i=0;i<(stacks - 1);i++) {
// 		for(j=0;j<slices;j++) {
// 			indices[j*2+0] = i + 0 + j * stacks;
// 			indices[j*2+1] = i + 1 + j * stacks;
// 		}
// 		glDrawElements( GL_TRIANGLE_STRIP, (slices)*2, GL_UNSIGNED_SHORT, indices );
// 	}
// 
//     glDisableVertexAttribArray( mVertex );
//     glDisableVertexAttribArray( mTexCoord );
//     glDisableVertexAttribArray( mNormal );
// 
// 	delete [] normal;
// 	delete [] tex;
// 	delete [] vertex;
// 	delete [] indices;
// }
// 
// namespace {
//     //  Helper to convert a vector of Vec2f to Vec3f, minimizing copies
//     void v3Fromv2(std::vector<Vec3f>& v3, std::vector<Vec2f>& v2)
//     {
//         std::vector<Vec3f> result;
//         for (std::vector<Vec2f>::iterator it = v2.begin(); it != v2.end(); ++it) {
//             result.push_back(Vec3f(it->x, it->y, 0));
//         }
//         v3.swap(result);
//     }
// }
// 
// // void GlesAttr::draw( const class PolyLine<Vec2f> &polyLine )
// // {
// // 
// //     glEnableVertexAttribArray(mVertex);
// //     glVertexAttribPointer( mVertex, 2, GL_FLOAT, GL_FALSE, 0, &(polyLine.getPoints()[0]) );
// //     glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
// //     glDisableVertexAttribArray( mVertex );
// // }
// 
// 
// void GlesAttr::draw( const class PolyLine<Vec3f> &polyLine )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &(polyLine.getPoints()[0]) );
//     glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
//     glDisableVertexAttribArray( mVertex );
// }
// 
// void GlesAttr::draw( const class Path2d &path2d, float approximationScale )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
// 	if( path2d.getNumSegments() == 0 )
// 		return;
// 	std::vector<Vec2f> points2f = path2d.subdivide( approximationScale );
//     std::vector<Vec3f> points;
//     v3Fromv2( points, points2f );
//     glEnableVertexAttribArray(mVertex);
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
// 	glDrawArrays( GL_LINE_STRIP, 0, points.size() );
//     glDisableVertexAttribArray( mVertex );
// }
// 
// 
// void GlesAttr::draw( const class Shape2d &shape2d, float approximationScale )
// {
//     selectAttrs( ES2_ATTR_VERTEX );
// 
//     glEnableVertexAttribArray(mVertex);
// 	for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
// 		if( contourIt->getNumSegments() == 0 )
// 			continue;
// 		std::vector<Vec2f> points2f = contourIt->subdivide( approximationScale );
//         std::vector<Vec3f> points;
//         v3Fromv2( points, points2f );
//         glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
// 		glDrawArrays( GL_LINE_STRIP, 0, points.size() );
// 	}
//     glDisableVertexAttribArray( mVertex );
// }
// 
// 
// 
// // void GlesAttr::drawSolid( const class Path2d &path2d, float approximationScale )
// // {
// // 
// // 	if( path2d.getNumSegments() == 0 )
// // 		return;
// // 	std::vector<Vec2f> points = path2d.subdivide( approximationScale );
// //     glEnableVertexAttribArray(mVertex);
// //     glVertexAttribPointer( mVertex, 2, GL_FLOAT, GL_FALSE, 0, &(points[0]) );
// // 	glDrawArrays( GL_POLYGON, 0, points.size() );
// //     glDisableVertexAttribArray( mVertex );
// // }
// 
// 
// 
// void GlesAttr::draw( const TriMesh &mesh )
// {
//     selectAttrs( ES2_ATTR_VERTEX | useTexCoordFlag() | ES2_ATTR_COLOR | ES2_ATTR_NORMAL );
// 
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getVertices()[0]) );
//     glEnableVertexAttribArray(mVertex);
// 
// 	if( mesh.hasNormals() ) {
//         glVertexAttribPointer( mNormal, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.getNormals()[0]) );
//         glEnableVertexAttribArray(mNormal);
// 	}
// 	else
// 		glDisableVertexAttribArray(mNormal);
// 	
// 	if( mesh.hasColorsRGB() ) {
//         
//         glVertexAttribPointer( mColor, 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, &(mesh.getColorsRGB()[0]) );
//         glEnableVertexAttribArray(mColor);
// 	}
// 	else if( mesh.hasColorsRGBA() ) {
//         glVertexAttribPointer( mColor, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, &(mesh.getColorsRGBA()[0]) );
//         glEnableVertexAttribArray(mColor);
// 	}
// 	else 
//         glDisableVertexAttribArray(mColor);
// 
// 	if( mesh.hasTexCoords() ) {
//         glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, &(mesh.getTexCoords()[0]) );
// 		glEnableVertexAttribArray( mTexCoord );
// 	}
// 	else
// 		glDisableVertexAttribArray( mTexCoord );
// 	glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
// 
//     glDisableVertexAttribArray( mVertex );
//     glDisableVertexAttribArray( mNormal );
//     glDisableVertexAttribArray( mColor );
//     glDisableVertexAttribArray( mTexCoord );
// }
// 
// 
// void GlesAttr::drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
// {
// }
// 
// 
// void GlesAttr::draw( const VboMesh &vbo )
// {
// }
// 
// 
// void GlesAttr::drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
// {
// }
// 
// void GlesAttr::drawArrays( const VboMesh &vbo, GLint first, GLsizei count )
// {
// }
// 
// void GlesAttr::drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
// {
//     selectAttrs( ES2_ATTR_VERTEX | useTexCoordFlag() );
// 
//     glEnableVertexAttribArray(mVertex);
// 	Vec3f verts[4];
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, &verts[0] );
//     glEnableVertexAttribArray(mTexCoord);
// 	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
// 
// 	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
// 	float cosA = math<float>::cos( toRadians( rotationDegrees ) );
// 
// 	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
// 	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
// 	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
// 	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
// 
// 	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
// 
//     glDisableVertexAttribArray( mVertex );
//     glDisableVertexAttribArray( mTexCoord );
// }
// 
// 
// void GlesAttr::draw( const Texture &texture )
// {
// 	draw( texture, Area( texture.getCleanBounds() ), texture.getCleanBounds() );
// }
// 
// 
// void GlesAttr::draw( const Texture &texture, const Vec2f &pos )
// {
// 	draw( texture, texture.getCleanBounds(), Rectf( pos.x, pos.y, pos.x + texture.getCleanWidth(), pos.y + texture.getCleanHeight() ) );
// }
// 
// 
// void GlesAttr::draw( const Texture &texture, const Rectf &rect )
// {
// 	draw( texture, texture.getCleanBounds(), rect );
// }
// 
// 
// void GlesAttr::draw( const Texture &texture, const Area &srcArea, const Rectf &destRect )
// {
//     selectAttrs( ES2_ATTR_VERTEX | ES2_ATTR_TEXCOORD );
// 
//     SaveTextureBindState saveBindState( texture.getTarget() );
//     BoolState saveEnabledState( texture.getTarget() );
//     ClientBoolState vertexArrayState( *this, GL_VERTEX_ARRAY );
//     ClientBoolState texCoordArrayState( *this, GL_TEXTURE_COORD_ARRAY );	
//     texture.enableAndBind();
//     glUniform1i(mTexSampler, 0);
// 
//     glEnableVertexAttribArray(mVertex);
//     GLfloat verts[12];
//     glVertexAttribPointer( mVertex, 3, GL_FLOAT, GL_FALSE, 0, verts );
//     glEnableVertexAttribArray(mTexCoord);
//     GLfloat texCoords[8];
//     glVertexAttribPointer( mTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
// 
//     verts[0*3+0] = destRect.getX2(); verts[0*3+1] = destRect.getY1(); verts[0*3+2] = 0;
//     verts[1*3+0] = destRect.getX1(); verts[1*3+1] = destRect.getY1(); verts[1*3+2] = 0;
//     verts[2*3+0] = destRect.getX2(); verts[2*3+1] = destRect.getY2(); verts[2*3+2] = 0;
//     verts[3*3+0] = destRect.getX1(); verts[3*3+1] = destRect.getY2(); verts[3*3+2] = 0;
// 
//     const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
//     texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();	
//     texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();	
//     texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();	
//     texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();	
// 
//     glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
// }
// 
// <<<<<<< HEAD
// =======
// class CinderProgES2 : public GlslProg
// {
// public:
//     static const char* verts; 
//     static const char* frags; 
// 
//     CinderProgES2() : GlslProg(verts, frags) 
//     { }
// };
// 
// const char* CinderProgES2::verts = 
//         "attribute vec3 aPosition;\n"
//         "attribute vec4 aColor;\n"
//         "attribute vec2 aTexCoord;\n"
// 
//         "uniform mat4 uProjection;\n"
//         "uniform mat4 uModelView;\n"
//         "uniform vec4 uVertexColor;\n"
// 
//         "uniform bool uHasVertexAttr;\n"
//         "uniform bool uHasTexCoordAttr;\n"
//         "uniform bool uHasColorAttr;\n"
//         "uniform bool uHasNormalAttr;\n"
// 
//         "varying vec4 vColor;\n"
//         "varying vec2 vTexCoord;\n"
// 
//         "void main() {\n"
//         "  vColor = uVertexColor;\n"
//         "  if (uHasColorAttr) {\n"
//         "    vColor *= aColor;\n"
//         "  }\n"
//         "  if (uHasTexCoordAttr) {\n"
//         "    vTexCoord = aTexCoord;\n"
//         "  }\n"
//         "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n"
//         "}\n";
// 
// const char* CinderProgES2::frags = 
//         "precision mediump float;\n"
// 
//         "uniform bool uHasTexCoordAttr;\n"
//         "uniform sampler2D sTexture;\n"
// 
//         "varying vec4 vColor;\n"
//         "varying vec2 vTexCoord;\n"
// 
//         "void main() {\n"
//         "    if (uHasTexCoordAttr) {\n"
//         "      gl_FragColor = vColor * texture2D(sTexture, vTexCoord);\n"
//         "    }\n"
//         "    else {\n"
//         "      gl_FragColor = vColor;\n"
//         "    }\n"
//         "}\n";
// 
// GlesContextRef GlesContext::create()
// {
//     return GlesContextRef(new GlesContext());
// }
// 
// GlesContextRef GlesContext::create(GlslProg& shader, GlesAttr& attr)
// {
//     return GlesContextRef(new GlesContext(shader, attr));
// }
// 
// GlesContext::GlesContext()
// {
//     CI_LOGW("Initializing CinderProgES2 shader");
// 
// 	try {
//         // CinderProgES2 shader;
//         GlslProg shader(CinderProgES2::verts, CinderProgES2::frags);
//         GlesAttr attr(shader.getAttribLocation("aPosition"),
//                       shader.getAttribLocation("aTexCoord"),
//                       shader.getAttribLocation("aColor"),
//                       shader.getAttribLocation("aNormal"));
//         attr.mTexSampler = shader.getUniformLocation("sTexture");
//         init(shader, attr);
//     }
//     catch( gl::GlslProgCompileExc &exc ) {
//         CI_LOGW("Shader compile error: \n");
//         CI_LOGW("%s\n", exc.what());
//     }
//     catch( ... ) {
//         CI_LOGW("Unable to load shader\n");
//     }
// }
// 
// GlesContext::GlesContext(GlslProg& shader, GlesAttr& attr)
// {
//     init(shader, attr);
// }
// 
// void GlesContext::init(GlslProg& shader, GlesAttr& attr)
// {
//     //  XXX use a setter
//     mProg = shader;
//     mAttr = attr;
//     mBound = false;
//     mAttr.mSelectAttr = this;
//     mColor = ColorA::white();
//     mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = true;
// }
// 
// void GlesContext::bind()
// {
//     if ( ! mBound ) {
//         mBound = true;
//         mProg.bind();
//     }
//     updateUniforms();
// }
// 
// void GlesContext::unbind()
// {
//     if ( mBound )
//         mProg.unbind();
// 
//     mBound = false;
//     mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = true;
//     mActiveAttrs = 0;
// }
// 
// void GlesContext::selectAttrs(uint32_t activeAttrs)
// {
//     if (mActiveAttrs != activeAttrs) {
//         // CI_LOGW("XXX New attributes set: %d", activeAttrs);
//         mActiveAttrsDirty = true;
//     }
//     mActiveAttrs = activeAttrs;
//     updateUniforms();
// }
// 
// GlesAttr& GlesContext::attr()
// {
//     return mAttr;
// }
// 
// void GlesContext::setMatrices( const Camera &cam )
// {
//     mModelView = cam.getModelViewMatrix();
//     mModelViewDirty = true;
//     mProj = cam.getProjectionMatrix();
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::setModelView( const Camera &cam )
// {
//     mModelView = cam.getModelViewMatrix();
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::setProjection( const Camera &cam )
// {
//     mProj = cam.getProjectionMatrix();
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::setProjection( const ci::Matrix44f &proj )
// {
//     mProj = proj;
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::pushModelView()
// {
//     mModelViewStack.push_back(mModelView);
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::popModelView()
// {
//     if (!mModelViewStack.empty()) {
//         mModelView = mModelViewStack.back();
//         mModelViewStack.pop_back();
//         mModelViewDirty = true;
//         updateUniforms();
//     }
// }
// 
// void GlesContext::pushModelView( const Camera &cam )
// {
//     mModelViewStack.push_back(cam.getModelViewMatrix().m);
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::pushProjection( const Camera &cam )
// {
//     mProjStack.push_back(mProj);
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::pushMatrices()
// {
//     mModelViewStack.push_back(mModelView);
//     mModelViewDirty = true;
//     mProjStack.push_back(mProj);
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::popMatrices()
// {
//     if (!mModelViewStack.empty()) {
//         mModelView = mModelViewStack.back();
//         mModelViewStack.pop_back();
//         mModelViewDirty = true;
//     }
//     if (!mProjStack.empty()) {
//         mProj = mProjStack.back();
//         mProjStack.pop_back();
//         mProjDirty = true;
//     }
// 
//     if (mModelViewDirty || mProjDirty)
//         updateUniforms();
// }
// 
// void GlesContext::multModelView( const Matrix44f &mtx )
// {
//     mModelView *= mtx;
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::multProjection( const Matrix44f &mtx )
// {
//     mProj *= mtx;
//     mProjDirty = true;
//     updateUniforms();
// }
// 
// Matrix44f GlesContext::getModelView()
// {
//     return mModelView;
// }
// 
// Matrix44f GlesContext::getProjection()
// {
//     return mProj;
// }
// 
// void GlesContext::setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
// {
// 	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );
// 
//     mProj = cam.getProjectionMatrix();
//     mModelView = cam.getModelViewMatrix();
// 
// 	if( originUpperLeft ) {
//         scale(Vec3f(1.0f, -1.0f, 1.0f));  // invert Y axis so increasing Y goes down.
//         translate(1.0f, -1.0f, 1.0f);     // shift origin up to upper-left corner.
// 		glViewport( 0, 0, screenWidth, screenHeight );
// 	}
// 
//     mProjDirty = mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft)
// {
//     CameraOrtho cam;
// 
//     if (originUpperLeft) {
//         cam.setOrtho(0, screenWidth, screenHeight, 0, 1.0f, -1.0f);
//     }
//     else {
//         cam.setOrtho(0, screenWidth, 0, screenHeight, 1.0f, -1.0f);
//     }
// 
//     mProj = cam.getProjectionMatrix();
//     mModelView = Matrix44f::identity();
//     glViewport( 0, 0, screenWidth, screenHeight );
// 
//     mProjDirty = mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::translate( const Vec2f &pos )
// {
//     mModelView.translate(Vec3f(pos.x, pos.y, 0));
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::translate( const Vec3f &pos )
// {
//     mModelView.translate(pos);
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::scale( const Vec3f &scl )
// {
//     mModelView.scale(scl);
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::rotate( const Vec3f &xyz )
// {
//     Vec3f xyzrad(toRadians(xyz.x), toRadians(xyz.y), toRadians(xyz.z));
//     mModelView.rotate(xyzrad);
//     mModelViewDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::rotate( const Quatf &quat )
// {
// 	Vec3f axis;
// 	float angle;
// 	quat.getAxisAngle( &axis, &angle );
//     if( math<float>::abs( angle ) > EPSILON_VALUE ) {
// 		mModelView.rotate( Vec3f(axis.x, axis.y, axis.z), angle );
//         mModelViewDirty = true;
//         updateUniforms();
//     }
// }
// 
// void GlesContext::color( float r, float g, float b )
// {
//     mColor = ColorA( r, g, b, 1.0f );
//     mColorDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::color( float r, float g, float b, float a )
// {
//     mColor = ColorA( r, g, b, a );
//     mColorDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::color( const Color &c )
// { 
//     mColor = c;
//     mColorDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::color( const ColorA &c ) 
// { 
//     mColor = c;
//     mColorDirty = true;
//     updateUniforms();
// }
// 
// void GlesContext::updateUniforms()
// {
//     if (!mBound)
//         return;
// 
//     if (mProjDirty)
//         mProg.uniform("uProjection", mProj);
// 
//     if (mModelViewDirty)
//         mProg.uniform("uModelView",  mModelView);
// 
//     if (mColorDirty)
//         mProg.uniform("uVertexColor", mColor);
// 
//     if (mActiveAttrsDirty) {
//         mProg.uniform("uHasVertexAttr",   bool(mActiveAttrs & ES2_ATTR_VERTEX));
//         mProg.uniform("uHasTexCoordAttr", bool(mActiveAttrs & ES2_ATTR_TEXCOORD));
//         mProg.uniform("uHasColorAttr",    bool(mActiveAttrs & ES2_ATTR_COLOR));
//         mProg.uniform("uHasNormalAttr",   bool(mActiveAttrs & ES2_ATTR_NORMAL));
//     }
// 
//     mProjDirty = mModelViewDirty = mColorDirty = mTextureDirty = mActiveAttrsDirty = false;
// }
// 
// static shared_ptr<GlesContext> sContext;
// 
// GlesContextRef setGlesContext(GlesContextRef context)
// {
//     releaseGlesContext();
// 
//     if (context) {
//         sContext = context;
//     }
//     else {
//         //  Use default context
//         sContext = GlesContext::create();
//     }
// 
//     return sContext;
// }
// 
// GlesContextRef getGlesContext()
// {
//     return sContext;
// }
// 
// void releaseGlesContext()
// {
//     sContext = GlesContextRef();
// }
// 
// >>>>>>> android
// void drawLine( const Vec2f &start, const Vec2f &end )
// {
//     if (sContext) sContext->attr().drawLine(start, end);
// }
// 
// void drawLine( const Vec3f &start, const Vec3f &end )
// {
//     if (sContext) sContext->attr().drawLine(start, end);
// }
// 
// void drawCube( const Vec3f &center, const Vec3f &size )
// {
//     if (sContext) sContext->attr().drawCube(center, size);
// }
// 
// void drawColorCube( const Vec3f &center, const Vec3f &size )
// {
//     if (sContext) sContext->attr().drawColorCube(center, size);
// }
// 
// void drawStrokedCube( const Vec3f &center, const Vec3f &size )
// {
//     if (sContext) sContext->attr().drawStrokedCube(center, size);
// }
// 
// void drawSphere( const Vec3f &center, float radius, int segments )
// {
//     if (sContext) sContext->attr().drawSphere(center, radius, segments);
// }
// 
// void draw( const class Sphere &sphere, int segments )
// {
//     if (sContext) sContext->attr().draw(sphere, segments);
// }
// 
// void drawSolidCircle( const Vec2f &center, float radius, int numSegments )
// {
//     if (sContext) sContext->attr().drawSolidCircle(center, radius, numSegments);
// }
// 
// void drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
// {
//     if (sContext) sContext->attr().drawStrokedCircle(center, radius, numSegments);
// }
// 
// void drawSolidRect( const Rectf &rect, bool textureRectangle )
// {
//     if (sContext) sContext->attr().drawSolidRect(rect, textureRectangle);
// }
// 
// void drawStrokedRect( const Rectf &rect )
// {
//     if (sContext) sContext->attr().drawStrokedRect(rect);
// }
// 
// void drawCoordinateFrame( float axisLength, float headLength, float headRadius )
// {
//     if (sContext) sContext->attr().drawCoordinateFrame(axisLength, headLength, headRadius);
// }
// 
// void drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
// {
//     if (sContext) sContext->attr().drawVector(start, end, headLength, headRadius);
// }
// 
// void drawFrustum( const Camera &cam )
// {
//     if (sContext) sContext->attr().drawFrustum(cam);
// }
// 
// void drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
// {
//     if (sContext) sContext->attr().drawTorus(outterRadius, innerRadius, longitudeSegments, latitudeSegments);
// }
// 
// void drawCylinder( float baseRadius, float topRadius, float height, int slices, int stacks )
// {
//     if (sContext) sContext->attr().drawCylinder(baseRadius, topRadius, height, slices, stacks);
// }
// 
// // void draw( const class PolyLine<Vec2f> &polyLine )
// // {
// // }
// //
// 
// void draw( const class PolyLine<Vec3f> &polyLine )
// {
//     if (sContext) sContext->attr().draw(polyLine);
// }
// 
// void draw( const class Path2d &path2d, float approximationScale )
// {
//     if (sContext) sContext->attr().draw(path2d, approximationScale);
// }
// 
// void draw( const class Shape2d &shape2d, float approximationScale )
// {
//     if (sContext) sContext->attr().draw(shape2d, approximationScale);
// }
// 
// // void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f )
// // {
// // }
// //
// 
// void draw( const TriMesh &mesh )
// {
//     if (sContext) sContext->attr().draw(mesh);
// }
// 
// // void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
// // {
// //     if (sContext) sContext->attr().draw(mesh, startTriangle, triangleCount);
// // }
// 
// void draw( const VboMesh &vbo )
// {
//     if (sContext) sContext->attr().draw(vbo);
// }
// 
// void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
// {
//     if (sContext) sContext->attr().drawRange(vbo, startIndex, indexCount, vertexStart, vertexEnd);
// }
// 
// void drawArrays( const VboMesh &vbo, GLint first, GLsizei count )
// {
//     if (sContext) sContext->attr().drawArrays(vbo, first, count);
// }
// 
// void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
// {
//     if (sContext) sContext->attr().drawBillboard(pos, scale, rotationDegrees, bbRight, bbUp);
// }
// 
// void draw( const Texture &texture )
// {
//     if (sContext) sContext->attr().draw(texture);
// }
// 
// void draw( const Texture &texture, const Vec2f &pos )
// {
//     if (sContext) sContext->attr().draw(texture, pos);
// }
// 
// void draw( const Texture &texture, const Rectf &rect )
// {
//     if (sContext) sContext->attr().draw(texture, rect);
// }
// 
// void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect )
// {
//     if (sContext) sContext->attr().draw(texture, srcArea, destRect);
// }
// 
// void setMatrices( const Camera &cam )
// {
//     if (sContext) sContext->setMatrices(cam);
// }
// 
// void setModelView( const Camera &cam )
// {
//     if (sContext) sContext->setModelView(cam);
// }
// 
// void setProjection( const Camera &cam )
// {
//     if (sContext) sContext->setProjection(cam);
// }
// 
// void setProjection( const Matrix44f &proj )
// {
//     if (sContext) sContext->setProjection(proj);
// }
// 
// 
// void pushModelView()
// {
//     if (sContext) sContext->pushModelView();
// }
// 
// void popModelView()
// {
//     if (sContext) sContext->popModelView();
// }
// 
// void pushModelView( const Camera &cam )
// {
//     if (sContext) sContext->pushModelView(cam);
// }
// 
// void pushProjection( const Camera &cam )
// {
//     if (sContext) sContext->pushProjection(cam);
// }
// 
// void pushMatrices()
// {
//     if (sContext) sContext->pushMatrices();
// }
// 
// void popMatrices()
// {
//     if (sContext) sContext->popMatrices();
// }
// 
// void multModelView( const Matrix44f &mtx )
// {
//     if (sContext) sContext->multModelView(mtx);
// }
// 
// void multProjection( const Matrix44f &mtx )
// {
//     if (sContext) sContext->multProjection(mtx);
// }
// 
// Matrix44f getModelView()
// {
//     if (sContext) sContext->getModelView();
// }
// 
// Matrix44f getProjection()
// {
//     if (sContext) sContext->getProjection();
// }
// 
// void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
// {
//     if (sContext) sContext->setMatricesWindowPersp(screenWidth, screenHeight, fovDegrees, nearPlane, farPlane, originUpperLeft);
// }
// 
// void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
// {
//     if (sContext) sContext->setMatricesWindow(screenWidth, screenHeight, originUpperLeft);
// }
// 
// void translate( const Vec2f &pos )
// {
//     if (sContext) sContext->translate(pos);
// }
// 
// void translate( const Vec3f &pos )
// {
//     if (sContext) sContext->translate(pos);
// }
// 
// void scale( const Vec3f &scl )
// {
//     if (sContext) sContext->scale(scl);
// }
// 
// void rotate( const Vec3f &xyz )
// {
//     if (sContext) sContext->rotate(xyz);
// }
// 
// void rotate( const Quatf &quat )
// {
//     if (sContext) sContext->rotate(quat);
// }
// 
// void color( float r, float g, float b )
// {
//     if (sContext) sContext->color(r, g, b);
// }
// 
// void color( float r, float g, float b, float a )
// {
//     if (sContext) sContext->color(r, g, b, a);
// }
// 
// // void color( const Color8u &c );
// // void color( const ColorA8u &c );
// void color( const Color &c )
// {
//     if (sContext) sContext->color(c);
// }
// 
// void color( const ColorA &c )
// {
//     if (sContext) sContext->color(c);
// }
// 
// ClientBoolState::ClientBoolState( GLint target )
// {
//     GLES2ContextRef context = getGLES2Context();
// 
//     //  Does nothing if there's no context set
//     if (context) {
//         init( getGLES2Context()->attr(), target );
//     }
// }
// 
// ClientBoolState::ClientBoolState( GlesAttr& attr, GLint target )
// {
//     init( attr, target);
// }
// 
// void ClientBoolState::init(GlesAttr& attr, GLint target )
// {
//     switch( target ) {
//     case GL_VERTEX_ARRAY:
//         mTarget = attr.mVertex;
//         break;
//     case GL_COLOR_ARRAY:
//         mTarget = attr.mColor;
//         break;
//     case GL_TEXTURE_COORD_ARRAY:
//         mTarget = attr.mTexCoord;
//         break;
//     case GL_NORMAL_ARRAY:
//         mTarget = attr.mNormal;
//         break;
//     default:
//         mTarget = 0;
//     }
// 
//     if (mTarget)
//         glGetVertexAttribiv(attr.mVertex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &mOldValue);
// }
// 
// ClientBoolState::~ClientBoolState()
// {
//     if (!mTarget)
//         return;
// 
//     if (mOldValue) {
//         glEnableVertexAttribArray(mTarget);
//     }
//     else {
//         glDisableVertexAttribArray(mTarget);
//     }
// }
// 
// } }
// 
// 
