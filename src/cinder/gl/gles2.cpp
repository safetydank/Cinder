//  GLES2 context implementation

#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

namespace cinder { namespace gl { 

namespace context {

class ES2Context;
typedef std::shared_ptr<ES2Context> ES2ContextRef;

const char* defaultVertex = 
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

const char* defaultFrag = 
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


enum {
    ATTR_POSITION = 0,
    ATTR_NORMAL,
    ATTR_COLOR,
    ATTR_TEXCOORD0,
    ATTR_TEXCOORD1,
    ATTR_TEXCOORD2,
    ATTR_COUNT
};


struct ES2Attribute
{
    int     size;
    GLenum  type;
    GLsizei stride;
    void*   pointer;

    int   id;
    GLint location;
    bool  enabled;
    bool  uploaded;

    ES2Attribute() 
        : size(0), type(0), stride(0), pointer(0), id(0), location(0), enabled(false), uploaded(false)
    {
    }

    void set(GLint asize, GLenum atype, GLsizei astride, void* apointer)
    {
        size     = asize;
        type     = atype;
        stride   = astride;
        pointer  = apointer;
        uploaded = false;
    }

    void upload()
    {
        if (enabled) {
            glEnableVertexAttribArray(location);
            if (!uploaded) {
                glVertexAttribPointer(location, size, type, normalized, stride, pointer);
                uploaded = true;
            }
        } else {
            glDisableVertexAttribArray(location);
        }
    }
};

struct ES2UniformBase
{
};

struct ES2Uniform
{
};

class ES2Context
{
protected:
    GlslProg mShader;
    std::vector<ES2Attribute> mAttributes;

    std::vector<Matrix44f> mModelViewStack;
    std::vector<Matrix44f> mProjStack;

    Matrix44f mProj;
    Matrix44f mModelView;

    ColorA    mColor;
    int       mActiveTexture;
    int       mClientActiveTexture;
    Texture   mTexture;

    bool mLightingEnabled;

    bool mProjDirty;
    bool mModelViewDirty;

    ES2Context() : mActiveTexture(0)
    {
        mShader = GlslProg(defaultVertex, defaultFrag);
        for (int i=0; i < ATTR_COUNT; ++i) {
            mAttributes.push_back(ES2Attribute());
        }
    }

    int attributeId(GLenum cap)
    {
        int capId;
        switch (cap) {
            case GL_VERTEX_ARRAY:
                capId = ATTR_POSITION;
                break;
            case GL_NORMAL_ARRAY:
                capId = ATTR_NORMAL;
                break;
            case GL_COLOR_ARRAY:
                capId = ATTR_COLOR;
                break;
            case GL_TEXTURE_COORD_ARRAY:
                capId = ATTR_TEXCOORD0 + mActiveTexture;
                break;
            default:
                capId = -1;
                break;
        }

        return capId;
    }

    void preDraw()
    {
        Matrix44f modelView = mModelViewStack.back();
    }

    //  Update shader uniforms
    void updateUniforms()
    {
    }

public:
    //  Singleton
    static ES2ContextRef sContext;

    ~ES2Context()
    {
        mShader.reset();
    }

    void initialize()
    {
        CI_LOGW("XXX context::initialize() GLES2");
        sContext.reset();
        sContext = ES2ContextRef(new ES2Context());
    }

    void bind()
    {
        mShader.bind();
    }

    void unbind()
    {
        mShader.unbind();
    }

    void color( float r, float g, float b ) { mColor = Color(r, g, b); }
    void color( float r, float g, float b, float a ) { mColor = ColorA(r, g, b, a); }
    void color( const Color8u &c ) { mColor = c; }
    void color( const ColorA8u &c ) { mColor = c; }
    void color( const Color &c ) { mColor = c; }
    void color( const ColorA &c ) { mColor = c; }

    void enableClientState(GLenum cap)
    {
        int capId = attributeId(cap);
        if (capId >= 0)
            mAttributes[capId].enabled = true;
    }
    
    void disableClientState(GLenum cap)
    {
        int capId = attributeId(cap);
        if (capId >= 0)
            mAttributes[capId].enabled = false;
    }

    void enableLighting()
    {
        mLightingEnabled = true;
    }

    void disableLighting()
    {
        mLightingEnabled = false;
    }

    void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
    {
        mAttributes[ATTR_POSITION]->set(size, type, stride, pointer);
    }

    void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
    {
        mAttributes[ATTR_TEXCOORD0 + mClientActiveTexture]->set(size, type, stride, pointer);
    }

    void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
    {
        mAttributes[ATTR_COLOR]->set(size, type, stride, pointer);
    }

    void normalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
    {
        int size = 3;
        mAttributes[ATTR_NORMAL]->set(size, type, stride, pointer);
    }

    void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
    {
        updateUniforms();
        glDrawElements(mode, count, type, indices);
    }

    void drawArrays(GLenum mode, GLint first, GLsizei count)
    {
        updateUniforms();
        glDrawArrays(mode, first, count);
    }

    void drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
    {
        updateUniforms();
        glDrawElements(mode, count, type, indices);
    }

    void clientActiveTexture(GLenum texture)
    {
        mClientActiveTexture = texture - GL_TEXTURE0;
    }

    void setMatrices( const Camera &cam )
    {
        mModelView = cam.getModelViewMatrix();
        mModelViewDirty = true;
        mProj = cam.getProjectionMatrix();
        mProjDirty = true;
    }
    
    void setModelView( const Camera &cam )
    {
        mModelView = cam.getModelViewMatrix();
        mModelViewDirty = true;
    }
    
    void setProjection( const Camera &cam )
    {
        mProj = cam.getProjectionMatrix();
        mProjDirty = true;
    }
    
    void setProjection( const Matrix44f &proj )
    {
        mProj = proj;
        mProjDirty = true;
    }
    
    void pushModelView()
    {
        mModelViewStack.push_back(mModelView);
        mModelViewDirty = true;
    }
    
    void popModelView()
    {
        if (!mModelViewStack.empty()) {
            mModelView = mModelViewStack.back();
            mModelViewStack.pop_back();
            mModelViewDirty = true;
        }
    }
    
    void pushModelView( const Camera &cam )
    {
        mModelViewStack.push_back(cam.getModelViewMatrix().m);
        mModelViewDirty = true;
    }
    
    void pushProjection( const Camera &cam )
    {
        mProjStack.push_back(mProj);
        mProjDirty = true;
    }
    
    void pushMatrices()
    {
        mModelViewStack.push_back(mModelView);
        mModelViewDirty = true;
        mProjStack.push_back(mProj);
        mProjDirty = true;
    }
    
    void popMatrices()
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
    }
    
    void multModelView( const Matrix44f &mtx )
    {
        mModelView *= mtx;
        mModelViewDirty = true;
    }
    
    void multProjection( const Matrix44f &mtx )
    {
        mProj *= mtx;
        mProjDirty = true;
    }
    
    Matrix44f getModelView()
    {
        return mModelView;
    }
    
    Matrix44f getProjection()
    {
        return mProj;
    }

    void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
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
    }

    void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft)
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
    }

    void translate( const Vec2f &pos )
    {
        mModelView.translate(Vec3f(pos.x, pos.y, 0));
        mModelViewDirty = true;
    }

    void translate(float x, float y, float z)
    {
        translate(Vec3f(x, y, z));
    }

    void translate( const Vec3f &pos )
    {
        mModelView.translate(pos);
        mModelViewDirty = true;
    }

    void scale( const Vec3f &scl )
    {
        mModelView.scale(scl);
        mModelViewDirty = true;
    }

    void rotate( const Vec3f &xyz )
    {
        Vec3f xyzrad(toRadians(xyz.x), toRadians(xyz.y), toRadians(xyz.z));
        mModelView.rotate(xyzrad);
        mModelViewDirty = true;
    }

    void rotate( const Quatf &quat )
    {
        Vec3f axis;
        float angle;
        quat.getAxisAngle( &axis, &angle );
        if( math<float>::abs( angle ) > EPSILON_VALUE ) {
            mModelView.rotate( Vec3f(axis.x, axis.y, axis.z), angle );
            mModelViewDirty = true;
        }
    }

    ES2Attribute& attribute(int id) 
    {
        return mAttributes[id];
    }
};

//  Static context member
ES2ContextRef ES2Context::sContext;

//  Context implementation of gl.h methods delegate to static ES2Context instance
#define GLES_CONTEXT_METHOD(FN, ...) \
    assert(ci::gl::context::ES2Context::sContext); \
    ci::gl::context::ES2Context::sContext->FN(__VA_ARGS__);
#define GLES_CONTEXT_METHOD_R(FN, ...) \
    assert(ci::gl::context::ES2Context::sContext); \
    return ci::gl::context::ES2Context::sContext->FN(__VA_ARGS__);

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

void disableClientState(GLenum cap)
{
    GLES_CONTEXT_METHOD(disableClientState, cap)
}

void enableLighting()
{
    GLES_CONTEXT_METHOD(enableLighting)
}

void disableLighting()
{
    GLES_CONTEXT_METHOD(disableLighting)
}

void vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    GLES_CONTEXT_METHOD(vertexPointer, size, type, stride, pointer)
}

void texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    GLES_CONTEXT_METHOD(texCoordPointer, size, type, stride, pointer)
}

void colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    GLES_CONTEXT_METHOD(colorPointer, size, type, stride, pointer)
}

void normalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
    GLES_CONTEXT_METHOD(normalPointer, type, stride, pointer)
}

void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    GLES_CONTEXT_METHOD(drawElements, mode, count, type, indices)
}

void drawArrays(GLenum mode, GLint first, GLsizei count)
{
    GLES_CONTEXT_METHOD(drawArrays, mode, first, count)
}

void drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
    GLES_CONTEXT_METHOD(drawRangeElements, mode, start, end, count, type, indices)
}

void clientActiveTexture(GLenum texture)
{
    GLES_CONTEXT_METHOD(clientActiveTexture, texture)
}

void color( float r, float g, float b )
{
    GLES_CONTEXT_METHOD(color, r, g, b)
}

void color( float r, float g, float b, float a )
{
    GLES_CONTEXT_METHOD(color, r, g, b, a)
}

void color( const Color8u &c )
{
    GLES_CONTEXT_METHOD(color, c)
}

void color( const ColorA8u &c )
{
    GLES_CONTEXT_METHOD(color, c)
}

void color( const Color &c )
{
    GLES_CONTEXT_METHOD(color, c)
}

void color( const ColorA &c )
{
    GLES_CONTEXT_METHOD(color, c)
}

} // namespace context

void setMatrices( const Camera &cam )
{
    GLES_CONTEXT_METHOD(setMatrices, cam)
}

void setModelView( const Camera &cam )
{
    GLES_CONTEXT_METHOD(setModelView, cam)
}

void setProjection( const Camera &cam )
{
    GLES_CONTEXT_METHOD(setProjection, cam)
}

void setProjection( const Matrix44f &proj )
{
    GLES_CONTEXT_METHOD(setProjection, proj)
}

void pushModelView()
{
    GLES_CONTEXT_METHOD(pushModelView)
}

void popModelView()
{
    GLES_CONTEXT_METHOD(popModelView)
}

void pushModelView( const Camera &cam )
{
    GLES_CONTEXT_METHOD(pushModelView, cam)
}

void pushProjection( const Camera &cam )
{
    GLES_CONTEXT_METHOD(pushProjection, cam)
}

void pushMatrices()
{
    GLES_CONTEXT_METHOD(pushMatrices)
}

void popMatrices()
{
    GLES_CONTEXT_METHOD(popMatrices)
}

void multModelView( const Matrix44f &mtx )
{
    GLES_CONTEXT_METHOD(multModelView, mtx)
}

void multProjection( const Matrix44f &mtx )
{
    GLES_CONTEXT_METHOD(multProjection, mtx)
}

Matrix44f getModelView()
{
    GLES_CONTEXT_METHOD_R(getModelView)
}

Matrix44f getProjection()
{
    GLES_CONTEXT_METHOD_R(getProjection)
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
    GLES_CONTEXT_METHOD(setMatricesWindowPersp, screenWidth, screenHeight, fovDegrees, nearPlane, farPlane, originUpperLeft)
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft)
{
    GLES_CONTEXT_METHOD(setMatricesWindow, screenWidth, screenHeight, originUpperLeft)
}

void translate( const Vec2f &pos )
{
    GLES_CONTEXT_METHOD(translate, pos)
}

void translate( const Vec3f &pos )
{
    GLES_CONTEXT_METHOD(translate, pos)
}

void scale( const Vec3f &scl )
{
    GLES_CONTEXT_METHOD(scale, scl)
}

void rotate( const Vec3f &xyz )
{
    GLES_CONTEXT_METHOD(rotate, xyz)
}

void rotate( const Quatf &quat )
{
    GLES_CONTEXT_METHOD(rotate, quat)
}

// XXX FIXME
ClientBoolState::ClientBoolState( GLint target )
{
    context::ES2ContextRef& context = context::ES2Context::sContext;
    //  Does nothing if there's no context set
    if (context) {
        switch( target ) {
            case GL_VERTEX_ARRAY:
                mTarget = 0;
                break;
            case GL_COLOR_ARRAY:
                mTarget = 0;
                break;
            case GL_TEXTURE_COORD_ARRAY:
                mTarget = 0;
                break;
            case GL_NORMAL_ARRAY:
                mTarget = 0;
                break;
            default:
                mTarget = 0;
        }

    if (mTarget)
        glGetVertexAttribiv(mTarget, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &mOldValue);
    }
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

} } // namespace cinder::gl


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
