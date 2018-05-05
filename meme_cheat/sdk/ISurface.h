#pragma once

namespace sdk {
	struct Vertex_t {
		Vertex_t( ) { }
		Vertex_t( const Vector2D &pos, const Vector2D &coord = Vector2D( 0, 0 ) ) {
			m_Position = pos;
			m_TexCoord = coord;
		}
		void Init( const Vector2D &pos, const Vector2D &coord = Vector2D( 0, 0 ) ) {
			m_Position = pos;
			m_TexCoord = coord;
		}

		Vector2D	m_Position;
		Vector2D	m_TexCoord;
	};
	struct IntRect {
		int x0;
		int y0;
		int x1;
		int y1;
		IntRect( int X0, int Y0, int X1, int Y1 ) {
			x0 = X0;
			y0 = Y0;
			x1 = X1;
			y0 = Y0;
		}
	};

	enum EFontFlags {
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400,
		FONTFLAG_BITMAP = 0x800,
	};
	enum FontDrawType_t {
		// Use the "additive" value from the scheme file
		FONT_DRAW_DEFAULT = 0,

		// Overrides
		FONT_DRAW_NONADDITIVE,
		FONT_DRAW_ADDITIVE,

		FONT_DRAW_TYPE_COUNT = 2,
	};

	class ISurface {
	public:
		void DrawSetTexture( int id ) {
			typedef void( __thiscall* Fn )( void*, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 38 ) )( this, id );
		}
		void DrawSetTextureRGBA( int TextureId, unsigned char const* rawRGBA, int W, int T, int h, bool f ) {
			typedef void( __thiscall* Fn )( void*, int, unsigned char const*, int, int, int, bool );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 37 ))( this, TextureId, rawRGBA, W, T, h, f );
		}
		void DrawSetTextureRGBA( int id, const unsigned char *rgba, int wide, int tall ) {
			typedef void( __thiscall* Fn )( void*, int, const unsigned char *, int, int, int, bool );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 37 ) )( this, id, rgba, wide, tall, 0, false );
		}

		void DrawTexturedPolygon( int n, Vertex_t *pVertice, bool bClipVertices = true ) {
			typedef void( __thiscall* Fn )( void*, int, Vertex_t*, bool );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 106 ) )( this, n, pVertice, bClipVertices );
		}
		bool IsTextureIDValid( int TextureID ) {
			typedef bool( __thiscall* Fn )( void*, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 42 ) )( this, TextureID );
		}
		void DrawSetColor( CColor col ) {
			typedef void( __thiscall* Fn )( void*, CColor );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 14 ) )( this, col );
		}
		int CreateNewTextureID( bool procedural = false ) {
			typedef int( __thiscall* Fn )( void*, bool );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 43 ) )( this, procedural );
		}
		void SetDrawColor( int r, int g, int b, int a ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 15 ) )( this, r, g, b, a );
		}
		void SetDrawColor( CColor color ) {
			typedef void( __thiscall* Fn )( void*, CColor );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 14 ) )( this, color );
		}
		void DrawFilledRect( int x, int y, int x2, int y2 ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 16 ) )( this, x, y, x2, y2 );
		}
		void DrawOutlinedRect( int x, int y, int x2, int y2 ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( VMT::VMTHookManager::GetFunction<Fn>( this, 18 ) )( this, x, y, x2, y2 );
		}
		bool GetTextSize( int Font, const wchar_t* _Input, int& Wide, int& Tall ) {
			typedef bool( __thiscall* Fn )( void*, int, const wchar_t*, int&, int& );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 79 ) )( this, Font, _Input, Wide, Tall );
		}

		void DrawLine( int x, int y, int x2, int y2 ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 19 ) )( this, x, y, x2, y2 );
		}

		void DrawPolyLine( int *px, int *py, int numPoints ) {
			typedef void( __thiscall* Fn )( void*, int*, int*, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 20 ) )( this, px, py, numPoints );
		}
		void DrawOutlinedCircle( int x, int y, int radius, int segments ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 103 ) )( this, x, y, radius, segments );
		}
		unsigned long SCreateFont( ) {
			typedef unsigned int( __thiscall* OriginalFn )( PVOID );
			return VMT::VMTHookManager::GetFunction< OriginalFn >( this, 71 )( this );
		}
		void SetFontGlyphSet( unsigned long& font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags ) {
			typedef void( __thiscall* OriginalFn )( PVOID, unsigned long, const char*, int, int, int, int, int, int, int );
			VMT::VMTHookManager::GetFunction< OriginalFn >( this, 72 )( this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0 );
		}
		void DrawFilledRectFade( int x0, int y0, int x1, int y1, size_t alpha0, size_t alpha1, bool horizontal ) {
			using func_ptr = void( __thiscall * )( void *, int, int, int, int, size_t, size_t, bool );
			VMT::VMTHookManager::GetFunction<func_ptr>( this, 123 )( this, x0, y0, x1, y1, alpha0, alpha1, horizontal );
		}
		void DrawSetTextFont( unsigned int Font ) {
			typedef void( __thiscall* Fn )( void*, unsigned int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 23 ) )( this, Font );
		}
		void DrawSetTextColor( int r, int g, int b, int a ) {
			typedef void( __thiscall* Fn )( void*, int, int, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 25 ) )( this, r, g, b, a );
		}
		void DrawSetTextPos( int x, int y ) {
			typedef void( __thiscall* Fn )( void*, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 26 ) )( this, x, y );
		}
		void DrawPrintText( const wchar_t* Text, int Len, int DrawType = 0 ) {
			typedef void( __thiscall* Fn )( void*, wchar_t const*, int, int );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 28 ) )( this, Text, Len, DrawType );
		}
		void DrawSetTextColor( CColor Color ) {
			typedef void( __thiscall* Fn )( void*, CColor );
			return ( ( Fn ) VMT::VMTHookManager::GetFunction<Fn>( this, 24 ) )( this, Color );
		}
		void DrawFilledRectArray( IntRect *rects, int rect_amount ) {
			typedef void( __thiscall *Fn )( void*, IntRect*, int );
			return VMT::VMTHookManager::GetFunction<Fn>( this, 17 )( this, rects, rect_amount );
		}
		void DrawTexturedRect( int x0, int y0, int x1, int y1 ) {
			typedef void( __thiscall *Fn )( void*, int, int, int, int );
			VMT::VMTHookManager::GetFunction<Fn>( this, 41 )( this, x0, y0, x1, y1 );
		}
		void IPlaySound( const char* fileName ) {
			typedef void( __thiscall* OriginalFn )( void*, const char* );
			return VMT::VMTHookManager::GetFunction<OriginalFn>( this, 82 )( this, fileName );
		}
	};
}