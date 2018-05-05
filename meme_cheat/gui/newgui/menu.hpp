#pragma once
#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IClient.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CGlobalVars.h"
#include <d3d9.h>
#include <Windows.h>

namespace gui {
	void initialize( );
	void shutdown( );

	void create_objects( );
	void destroy_objects( );

	void begin( );
	void render( );
	void end( );
}