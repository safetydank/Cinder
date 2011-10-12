#include "cinder/gl/gles2.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "cinder/Sphere.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/PolyLine.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"

using std::shared_ptr;

namespace cinder { namespace gl {

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

GlesContextRef GlesContext::create()
{
    return GlesContextRef(new GlesContext());
}

GlesContextRef GlesContext::create(GlslProg& shader, GlesAttr& attr)
{
    return GlesContextRef(new GlesContext(shader, attr));
}

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

static shared_ptr<GlesContext> sContext;

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

void drawLine( const Vec2f &start, const Vec2f &end )
{
    if (sContext) sContext->attr().drawLine(start, end);
}

void drawLine( const Vec3f &start, const Vec3f &end )
{
    if (sContext) sContext->attr().drawLine(start, end);
}

void drawCube( const Vec3f &center, const Vec3f &size )
{
    if (sContext) sContext->attr().drawCube(center, size);
}

void drawColorCube( const Vec3f &center, const Vec3f &size )
{
    if (sContext) sContext->attr().drawColorCube(center, size);
}

void drawStrokedCube( const Vec3f &center, const Vec3f &size )
{
    if (sContext) sContext->attr().drawStrokedCube(center, size);
}

void drawSphere( const Vec3f &center, float radius, int segments )
{
    if (sContext) sContext->attr().drawSphere(center, radius, segments);
}

void draw( const class Sphere &sphere, int segments )
{
    if (sContext) sContext->attr().draw(sphere, segments);
}

void drawSolidCircle( const Vec2f &center, float radius, int numSegments )
{
    if (sContext) sContext->attr().drawSolidCircle(center, radius, numSegments);
}

void drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
{
    if (sContext) sContext->attr().drawStrokedCircle(center, radius, numSegments);
}

void drawSolidRect( const Rectf &rect, bool textureRectangle )
{
    if (sContext) sContext->attr().drawSolidRect(rect, textureRectangle);
}

void drawStrokedRect( const Rectf &rect )
{
    if (sContext) sContext->attr().drawStrokedRect(rect);
}

void drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
    if (sContext) sContext->attr().drawCoordinateFrame(axisLength, headLength, headRadius);
}

void drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
    if (sContext) sContext->attr().drawVector(start, end, headLength, headRadius);
}

void drawFrustum( const Camera &cam )
{
    if (sContext) sContext->attr().drawFrustum(cam);
}

void drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
{
    if (sContext) sContext->attr().drawTorus(outterRadius, innerRadius, longitudeSegments, latitudeSegments);
}

void drawCylinder( float baseRadius, float topRadius, float height, int slices, int stacks )
{
    if (sContext) sContext->attr().drawCylinder(baseRadius, topRadius, height, slices, stacks);
}

// void draw( const class PolyLine<Vec2f> &polyLine )
// {
// }
//

void draw( const class PolyLine<Vec3f> &polyLine )
{
    if (sContext) sContext->attr().draw(polyLine);
}

void draw( const class Path2d &path2d, float approximationScale )
{
    if (sContext) sContext->attr().draw(path2d, approximationScale);
}

void draw( const class Shape2d &shape2d, float approximationScale )
{
    if (sContext) sContext->attr().draw(shape2d, approximationScale);
}

// void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f )
// {
// }
//

void draw( const TriMesh &mesh )
{
    if (sContext) sContext->attr().draw(mesh);
}

// void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
// {
//     if (sContext) sContext->attr().draw(mesh, startTriangle, triangleCount);
// }

void draw( const VboMesh &vbo )
{
    if (sContext) sContext->attr().draw(vbo);
}

void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
{
    if (sContext) sContext->attr().drawRange(vbo, startIndex, indexCount, vertexStart, vertexEnd);
}

void drawArrays( const VboMesh &vbo, GLint first, GLsizei count )
{
    if (sContext) sContext->attr().drawArrays(vbo, first, count);
}

void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
    if (sContext) sContext->attr().drawBillboard(pos, scale, rotationDegrees, bbRight, bbUp);
}

void draw( const Texture &texture )
{
    if (sContext) sContext->attr().draw(texture);
}

void draw( const Texture &texture, const Vec2f &pos )
{
    if (sContext) sContext->attr().draw(texture, pos);
}

void draw( const Texture &texture, const Rectf &rect )
{
    if (sContext) sContext->attr().draw(texture, rect);
}

void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect )
{
    if (sContext) sContext->attr().draw(texture, srcArea, destRect);
}

void setMatrices( const Camera &cam )
{
    if (sContext) sContext->setMatrices(cam);
}

void setModelView( const Camera &cam )
{
    if (sContext) sContext->setModelView(cam);
}

void setProjection( const Camera &cam )
{
    if (sContext) sContext->setProjection(cam);
}

void setProjection( const Matrix44f &proj )
{
    if (sContext) sContext->setProjection(proj);
}


void pushModelView()
{
    if (sContext) sContext->pushModelView();
}

void popModelView()
{
    if (sContext) sContext->popModelView();
}

void pushModelView( const Camera &cam )
{
    if (sContext) sContext->pushModelView(cam);
}

void pushProjection( const Camera &cam )
{
    if (sContext) sContext->pushProjection(cam);
}

void pushMatrices()
{
    if (sContext) sContext->pushMatrices();
}

void popMatrices()
{
    if (sContext) sContext->popMatrices();
}

void multModelView( const Matrix44f &mtx )
{
    if (sContext) sContext->multModelView(mtx);
}

void multProjection( const Matrix44f &mtx )
{
    if (sContext) sContext->multProjection(mtx);
}

Matrix44f getModelView()
{
    if (sContext) sContext->getModelView();
}

Matrix44f getProjection()
{
    if (sContext) sContext->getProjection();
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
    if (sContext) sContext->setMatricesWindowPersp(screenWidth, screenHeight, fovDegrees, nearPlane, farPlane, originUpperLeft);
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
    if (sContext) sContext->setMatricesWindow(screenWidth, screenHeight, originUpperLeft);
}

void translate( const Vec2f &pos )
{
    if (sContext) sContext->translate(pos);
}

void translate( const Vec3f &pos )
{
    if (sContext) sContext->translate(pos);
}

void scale( const Vec3f &scl )
{
    if (sContext) sContext->scale(scl);
}

void rotate( const Vec3f &xyz )
{
    if (sContext) sContext->rotate(xyz);
}

void rotate( const Quatf &quat )
{
    if (sContext) sContext->rotate(quat);
}

void color( float r, float g, float b )
{
    if (sContext) sContext->color(r, g, b);
}

void color( float r, float g, float b, float a )
{
    if (sContext) sContext->color(r, g, b, a);
}

// void color( const Color8u &c );
// void color( const ColorA8u &c );
void color( const Color &c )
{
    if (sContext) sContext->color(c);
}

void color( const ColorA &c )
{
    if (sContext) sContext->color(c);
}

ClientBoolState::ClientBoolState( GLint target )
{
    GlesContextRef context = getGlesContext();

    //  Does nothing if there's no context set
    if (context) {
        init( getGlesContext()->attr(), target );
    }
}

ClientBoolState::ClientBoolState( GlesAttr& attr, GLint target )
{
    init( attr, target);
}

void ClientBoolState::init(GlesAttr& attr, GLint target )
{
    switch( target ) {
    case GL_VERTEX_ARRAY:
        mTarget = attr.mVertex;
        break;
    case GL_COLOR_ARRAY:
        mTarget = attr.mColor;
        break;
    case GL_TEXTURE_COORD_ARRAY:
        mTarget = attr.mTexCoord;
        break;
    case GL_NORMAL_ARRAY:
        mTarget = attr.mNormal;
        break;
    default:
        mTarget = 0;
    }

    if (mTarget)
        glGetVertexAttribiv(attr.mVertex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &mOldValue);
}

ClientBoolState::~ClientBoolState()
{
    if (!mTarget)
        return;

    if (mOldValue) {
        glEnableVertexAttribArray(mTarget);
    }
    else {
        glDisableVertexAttribArray(mTarget);
    }
}

} }


