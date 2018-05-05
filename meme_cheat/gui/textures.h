#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock.h>
#include "../../includes.h"
#include "../utilities/interfaces.h"
#include "../sdk/ISurface.h"
#include <string.h>
#include "lodepng.h"

class texture {
public:
	int wi, he;
	texture( std::string );
	void draw( int, int, int, int );
private:
	unsigned char* chr;
	unsigned int last_t;
};
