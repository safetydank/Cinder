#include "TextureFont.h"
#include "Renderer.h"
#include "cinder/Text.h"

using namespace std;

namespace cinder { namespace pp {

TextureFontDrawRef TextureFontDraw::create(RendererRef renderer)
{
	return TextureFontDrawRef(new TextureFontDraw(renderer));
}

TextureFontDraw::TextureFontDraw(RendererRef renderer)
	: DrawBase(renderer)
{
}

TextureFontDraw::~TextureFontDraw()
{
}

void TextureFontDraw::drawString( TextureFont& texFont, const string &str, const Vec2f &baseline, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( TextBox::GROW, TextBox::GROW ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	texFont.drawGlyphs( *mRenderer, glyphMeasures, baseline, options );
}

void TextureFontDraw::drawString( TextureFont& texFont, const string &str, const Rectf &fitRect, const Vec2f &offset, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( TextBox::GROW, fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
	texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );	
}

#if defined( CINDER_COCOA ) || defined ( CINDER_ANDROID )
void TextureFontDraw::drawStringWrapped( TextureFont& texFont, const string &str, const Rectf &fitRect, const Vec2f &offset, const TextureFont::DrawOptions &options )
{
	TextBox tbox = TextBox().font( texFont.getFont() ).text( str ).size( fitRect.getWidth(), fitRect.getHeight() ).ligate( options.getLigate() );
	vector<pair<uint16_t,Vec2f> > glyphMeasures = tbox.measureGlyphs();
#if defined( CINDER_COCOA )
	texFont.drawGlyphs( glyphMeasures, fitRect.getUpperLeft() + offset, options );
#else
	// texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect, fitRect.getUpperLeft() + offset, options );

	// Clipping affects italicized fonts on Android, so use baseline
	// but have to compensate for rectangle upper border/ascent (?)
	texFont.drawGlyphs( *mRenderer, glyphMeasures, fitRect.getUpperLeft() + offset + Vec2f(0, texFont.getFont().getAscent() ), options );
#endif
}
#endif

TextureFont::TextureFont( const Font &font, const string &supportedChars, const Format &format )
	: gl::TextureFont( font, supportedChars, format )
{
}

TextureFont::TextureFont( const Font &font, const string &supportedChars, Atlas &atlas )
	: gl::TextureFont( font, supportedChars, atlas )
{
}

void TextureFont::drawGlyphs( Renderer& renderer, const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Vec2f &baselineIn, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );

	Vec2f baseline = baselineIn;

	const float scale = options.getScale();
	for( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			baseline = Vec2f( floor( baseline.x ), floor( baseline.y ) );
			
		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			
			Rectf destRect( glyphInfo.mTexCoords );
			Rectf srcCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( baseline.x, baseline.y - mFont.getAscent() * scale );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				
			
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY1() );
			verts.push_back( destRect.getX2() ); verts.push_back( destRect.getY2() );
			verts.push_back( destRect.getX1() ); verts.push_back( destRect.getY2() );

			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY1() );
			texCoords.push_back( srcCoords.getX2() ); texCoords.push_back( srcCoords.getY2() );
			texCoords.push_back( srcCoords.getX1() ); texCoords.push_back( srcCoords.getY2() );
			
			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}

			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		renderer.bindTexture(curTex);

		renderer.setPositionArray(&verts[0], 2);
		renderer.setTexCoordArray(&texCoords[0]);
		renderer.setColorArray(colors.empty() ? NULL : (GLubyte*) &vertColors[0]);

		renderer.enableClientState(texIdx == 0 ? Renderer::STATE_ALL : Renderer::UPLOAD_ATTRIBS);
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
		renderer.disableClientState();
	}
	renderer.unbindTexture();
}

void TextureFont::drawGlyphs( Renderer& renderer, const vector<pair<uint16_t,Vec2f> > &glyphMeasures, const Rectf &clip, Vec2f offset, const DrawOptions &options, const vector<ColorA8u> &colors )
{
	if( mTextures.empty() )
		return;

	if( ! colors.empty() ) {
		assert( glyphMeasures.size() == colors.size() );
	}
	else {
	}

	gl::SaveTextureBindState saveBindState( mTextures[0].getTarget() );
	gl::enable( mTextures[0].getTarget() );
	const float scale = options.getScale();

	for ( size_t texIdx = 0; texIdx < mTextures.size(); ++texIdx ) {
		vector<float> verts, texCoords;
		vector<ColorA8u> vertColors;
		const gl::Texture &curTex = mTextures[texIdx];
		vector<gl::index_t> indices;
		uint16_t curIdx = 0;
		if( options.getPixelSnap() )
			offset = Vec2f( floor( offset.x ), floor( offset.y ) );

		for( vector<pair<uint16_t,Vec2f> >::const_iterator glyphIt = glyphMeasures.begin(); glyphIt != glyphMeasures.end(); ++glyphIt ) {
			boost::unordered_map<Font::Glyph, GlyphInfo>::const_iterator glyphInfoIt = mGlyphMap.find( glyphIt->first );
			if( (glyphInfoIt == mGlyphMap.end()) || (mGlyphMap[glyphIt->first].mTextureIndex != texIdx) )
				continue;
				
			const GlyphInfo &glyphInfo = glyphInfoIt->second;
			Rectf srcTexCoords = curTex.getAreaTexCoords( glyphInfo.mTexCoords );
			Rectf destRect( glyphInfo.mTexCoords );
			destRect -= destRect.getUpperLeft();
			destRect.scale( scale );
			destRect += glyphIt->second * scale;
			destRect += Vec2f( floor( glyphInfo.mOriginOffset.x + 0.5f ), floor( glyphInfo.mOriginOffset.y ) ) * scale;
			destRect += Vec2f( offset.x, offset.y );
			if( options.getPixelSnap() )
				destRect -= Vec2f( destRect.x1 - floor( destRect.x1 ), destRect.y1 - floor( destRect.y1 ) );				

			// clip
			Rectf clipped( destRect );
			if( options.getClipHorizontal() ) {
				clipped.x1 = max( destRect.x1, clip.x1 );
				clipped.x2 = min( destRect.x2, clip.x2 );
			}
			if( options.getClipVertical() ) {
				clipped.y1 = max( destRect.y1, clip.y1 );
				clipped.y2 = min( destRect.y2, clip.y2 );
			}
			
			if( clipped.x1 >= clipped.x2 || clipped.y1 >= clipped.y2 )
				continue;
			
			Vec2f coordScale( 1 / (float)destRect.getWidth() / curTex.getWidth() * glyphInfo.mTexCoords.getWidth(),
				1 / (float)destRect.getHeight() / curTex.getHeight() * glyphInfo.mTexCoords.getHeight() );
			srcTexCoords.x1 = srcTexCoords.x1 + ( clipped.x1 - destRect.x1 ) * coordScale.x;
			srcTexCoords.x2 = srcTexCoords.x1 + ( clipped.x2 - clipped.x1 ) * coordScale.x;
			srcTexCoords.y1 = srcTexCoords.y1 + ( clipped.y1 - destRect.y1 ) * coordScale.y;
			srcTexCoords.y2 = srcTexCoords.y1 + ( clipped.y2 - clipped.y1 ) * coordScale.y;

			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY1() );
			verts.push_back( clipped.getX2() ); verts.push_back( clipped.getY2() );
			verts.push_back( clipped.getX1() ); verts.push_back( clipped.getY2() );

			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY1() );
			texCoords.push_back( srcTexCoords.getX2() ); texCoords.push_back( srcTexCoords.getY2() );
			texCoords.push_back( srcTexCoords.getX1() ); texCoords.push_back( srcTexCoords.getY2() );

			if( ! colors.empty() ) {
				for( int i = 0; i < 4; ++i )
					vertColors.push_back( colors[glyphIt-glyphMeasures.begin()] );
			}
			
			indices.push_back( curIdx + 0 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 2 );
			indices.push_back( curIdx + 2 ); indices.push_back( curIdx + 1 ); indices.push_back( curIdx + 3 );
			curIdx += 4;
		}
		
		if( curIdx == 0 )
			continue;
		
		renderer.bindTexture(curTex);

		renderer.setPositionArray(&verts[0], 2);
		renderer.setTexCoordArray(&texCoords[0]);
		renderer.setColorArray(colors.empty() ? NULL : (GLubyte*) &vertColors[0]);

		renderer.enableClientState(texIdx == 0 ? Renderer::STATE_ALL : Renderer::UPLOAD_ATTRIBS);
		glDrawElements( GL_TRIANGLES, indices.size(), CINDER_GL_INDEX_TYPE, &indices[0] );
	}
	renderer.disableClientState();
	renderer.unbindTexture();
}

TextureFontRef TextureFont::create( const Font &font, Atlas &atlas, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, atlas ) ); 
}

TextureFontRef TextureFont::create( const Font &font, const Format &format, const std::string &supportedChars )
{
	return TextureFontRef( new TextureFont( font, supportedChars, format ) ); 
}

} }  // namespace cinder::pp

