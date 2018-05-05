#include "../../includes.h"
#include "../utilities/interfaces.h"
#include "../sdk/IEngine.h"
#include "../sdk/CClientEntityList.h"
#include "../sdk/CInputSystem.h"
#include "../utilities/render.h"
#include "Components.h"
#include "../features/movement/Movement.h"
#include "../sdk/ConVar.h"
#include "menu_framework.h"
#define menu_color CColor(255, 140, 35);
#define font_color CColor(255, 140, 35)

namespace MENU {
	const std::string currentDateTime( ) {
		time_t     now = time( 0 );
		struct tm  tstruct;
		char       buf [ 80 ];
		tstruct = *localtime( &now );
		strftime( buf, sizeof( buf ), "%Y | %d | %X", &tstruct );

		return buf;
	}
	void InitColors( ) {
		using namespace PPGUI_PP_GUI;

		colors [ WINDOW_BODY ] = CColor( 22, 22, 22 );
		colors [ WINDOW_TITLE_BAR ] = CColor( 17, 17, 17 );
		colors [ WINDOW_TEXT ] = WHITE;

		colors [ GROUPBOX_BODY ] = CColor( 24, 24, 24 );
		colors [ GROUPBOX_OUTLINE ] = CColor( 48, 48, 48 );
		colors [ GROUPBOX_TEXT ] = WHITE;

		colors [ SCROLLBAR_BODY ] = CColor( 35, 35, 35, 255 );

		colors [ SEPARATOR_TEXT ] = WHITE;
		colors [ SEPARATOR_LINE ] = CColor( 35, 35, 35, 255 );

		colors [ CHECKBOX_CLICKED ] = menu_color
			colors [ CHECKBOX_NOT_CLICKED ] = CColor( 35, 35, 35, 255 );
		colors [ CHECKBOX_TEXT ] = WHITE;

		colors [ BUTTON_BODY ] = CColor( 35, 35, 35, 255 );
		colors [ BUTTON_TEXT ] = WHITE;

		colors [ COMBOBOX_TEXT ] = WHITE;
		colors [ COMBOBOX_SELECTED ] = CColor( 35, 35, 35, 255 );
		colors [ COMBOBOX_SELECTED_TEXT ] = WHITE;
		colors [ COMBOBOX_ITEM ] = CColor( 25, 25, 25, 255 );
		colors [ COMBOBOX_ITEM_TEXT ] = WHITE;

		colors [ SLIDER_BODY ] = CColor( 35, 35, 35, 255 );
		colors [ SLIDER_VALUE ] = menu_color
			colors [ SLIDER_TEXT ] = WHITE;

		colors [ TAB_BODY ] = CColor( 31, 31, 31, 255 );
		colors [ TAB_TEXT ] = CColor( 125, 125, 125, 255 );
		colors [ TAB_BODY_SELECTED ] = CColor( 31, 31, 31, 255 );
		colors [ TAB_TEXT_SELECTED ] = CColor( 255, 255, 255, 255 );

		colors [ VERTICAL_TAB_BODY ] = CColor( 70, 70, 70, 0 );
		colors [ VERTICAL_TAB_TEXT ] = WHITE;
		colors [ VERTICAL_TAB_OUTLINE ] = CColor( 0, 0, 0, 0 );
		colors [ VERTICAL_TAB_BODY_SELECTED ] = CColor( 100, 100, 100, 0 );

		colors [ COLOR_PICKER_BODY ] = CColor( 35, 35, 35, 255 );
		colors [ COLOR_PICKER_TEXT ] = WHITE;
		colors [ COLOR_PICKER_OUTLINE ] = CColor( 0, 0, 0, 100 );
	}
	void Do( ) {
		//all of this cool gui stuff made by bolbi, but i decide to just render text and make binds for every feature. sorry bolbi!

		if ( UTILS::INPUT::input_handler.GetKeyState( VK_INSERT ) & 1 ) {
			Vars::options.menu_open = !Vars::options.menu_open;
			ctx::engine->ClientCmd( Vars::options.menu_open ? "cl_mouseenable 0" :
				"cl_mouseenable 1" );
			ctx::input_system->EnableInput( !Vars::options.menu_open );
		}

		//--- Flip Bool Stuff ---//
			if ( ctx::input_system->IsButtonDown(( ButtonCode_t ) Vars::options.aa_right ) )
				Vars::options.flip_side = 0;

			if ( ctx::input_system->IsButtonDown( ( ButtonCode_t ) Vars::options.aa_left ) )
			Vars::options.flip_side = 1;

			if ( UTILS::INPUT::input_handler.GetKeyState( 0x58 ) )
				Vars::options.flip_side = Vars::options.flip_side == 1 ? 0 : 1;

			static size_t lastTime = 0;

			if ( ctx::input_system->IsButtonDown( ( ButtonCode_t ) Vars::options.tp_key ) ) {
				if ( GetTickCount( ) > lastTime ) {
					in_tp = !in_tp;
					lastTime = GetTickCount( ) + 400;
				}
		}			

		//--- New Menu Rendering System --//
		if ( menu_hide ) {
			//menu is hidden entirely, you can do anything here i guess
		}
		else {
			int w, h;
			ctx::engine->GetScreenSize( w, h );
			RENDER::DrawFilledRect( w - 182, 4, 182 + w - 187, 20, CColor( 24, 24, 24, 180 ));
			RENDER::DrawF( w - 187 + 7, 5, FONTS::menu_window_font, false, false, CColor( 255, 255, 255, 255 ), "gamesneeze beta |" );
			RENDER::DrawF( w - 187 + 92, 5, FONTS::menu_window_font, false, false, CColor( 255, 255, 255, 255 ), currentDateTime() );
		}
	}
}