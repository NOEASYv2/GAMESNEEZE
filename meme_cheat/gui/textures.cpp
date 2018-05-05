#include "textures.h"



std::vector <int> get_resulution( std::string file_name ) {

	std::ifstream in( file_name );

	unsigned int width, height;

	in.seekg( 16 );

	in.read( ( char * ) &width, 4 );

	in.read( ( char * ) &height, 4 );

	width = ntohl( width );

	height = ntohl( height );

	std::vector <int> kek;

	kek.push_back( width );

	kek.push_back( height );

	return kek;

}

// Main loading function 
unsigned char* ldImg( const char* filename, int w, int h ) {

	std::vector<unsigned char> image;

	unsigned width, height;

	width = w;

	height = h;

	unsigned error = lodepng::decode( image, width, height, filename );

	// Just incase we get errors display them and say something is wrong
	if ( error ) std::cout << "decoder error " << error << ": " << lodepng_error_text( error ) << std::endl;

	unsigned char* a = new unsigned char [ image.size( ) ];

	std::copy( image.begin( ), image.end( ), a );

	image.clear( );

	return a;

}

/*  Texture class content */

// We need that
void stringConvertToChar( std::string&str, char arr [ ], size_t arrSize )
//To convert std::string to char[]
{
	strcpy_s( arr, arrSize, str.c_str( ) );
	return;
};

// Make a pointer using texture * texturename and init it with texturename = new texture("pngname.png") which must bne located inside of the csgo folder
texture::texture( std::string fileName ) {

	std::vector <int> res = get_resulution( fileName );

	wi = res [ 0 ];

	he = res [ 1 ];

	char name [ MAX_PATH ];

	stringConvertToChar( fileName, name, MAX_PATH );

	chr = ldImg( name, wi, he );

	// unsigned int
	last_t = 0;

}

// Drawing function for our pictures
//                      X      Y      W        H
void texture::draw( int x, int y, int x_2, int y_2 ) {

	bool m_bValid = false;

	unsigned int m_iTexture;

	if ( last_t = 0 || !ctx::surface->IsTextureIDValid( last_t ) )

	{
		m_iTexture = ctx::surface->CreateNewTextureID( true );

		if ( m_iTexture )
			return;

		ctx::surface->DrawSetTextureRGBA( m_iTexture, chr, ( unsigned int ) wi, ( unsigned int ) he, 0, 1 );

		if ( ctx::surface->IsTextureIDValid( m_iTexture ) ) m_bValid = true;
		last_t = m_iTexture;

	}
	else {
		m_iTexture = last_t;

	}
	ctx::surface->DrawSetColor( CColor( 255, 255, 255, 255 ) );

	ctx::surface->DrawSetTexture( m_iTexture );

	ctx::surface->DrawTexturedRect( x, y, x_2, y_2 );

}


