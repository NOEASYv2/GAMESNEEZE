#include "menu.hpp"
#include "imgui\ColorVar.h"

std::vector<std::pair<int, const char*>> ButtonNamesA =
{
	{ 0,	"none" },
{ 1, "0" },
{ 2, "1" },
{ 3,"2" },
{ 4,"3" },
{ 5,"4" },
{ 6,"5" },
{ 7,"6" },
{ 8,"7" },
{ 9,	"8" },
{ 10,"9" },
{ 11,"a" },
{ 12,"b" },
{ 13,"c" },
{ 14,"d" },
{ 15,"e" },
{ 16,"f" },
{ 17,"g" },
{ 18,"h" },
{ 19,"i" },
{ 20,"j" },
{ 21,"k" },
{ 22,"l" },
{ 23,"m" },
{ 24,"n" },
{ 25,"o" },
{ 26,"p" },
{ 27,"q" },
{ 28,"r" },
{ 29,"s" },
{ 30,"t" },
{ 31,"u" },
{ 32,"v" },
{ 33,"w" },
{ 34,"x" },
{ 35,"y" },
{ 36,"z" },
{ 37,"pad_0" },
{ 38,"pad_1" },
{ 39,"pad_2" },
{ 40,"pad_3" },
{ 41,"pad_4" },
{ 42,"pad_5" },
{ 43,"pad_6" },
{ 44,"pad_7" },
{ 45,"pad_8" },
{ 46,"pad_9" },
{ 47,"pad_divide" },
{ 48,"pad_multiply" },
{ 49,"pad_minus" },
{ 50,"pad_plus" },
{ 51,"pad_enter" },
{ 52,"pad_decimal" },
{ 53,"lbracket" },
{ 54,	"rbracket" },
{ 55,"semicolon" },
{ 56,"apostrophe" },
{ 57,"backquote" },
{ 58,"comma" },
{ 59,"period" },
{ 60,"slash" },
{ 61,"backslash" },
{ 62,"minus" },
{ 63,"equal" },
{ 64,"enter" },
{ 65,"space" },
{ 66,"backspace" },
{ 67,"tab" },
{ 68,"capslock" },
{ 69,"numlock" },
{ 70,"escape" },
{ 71,	"scrolllock" },
{ 72,"insert" },
{ 73,"delete" },
{ 74,	"home" },
{ 75,	"end" },
{ 76,	"pageup" },
{ 77,	"pagedown" },
{ 78,	"break" },
{ 79,	"lshift" },
{ 80,	"rshift" },
{ 81,	"lalt" },
{ 82,	"ralt" },
{ 83,	"lcontrol" },
{ 84,	"rcontrol" },
{ 85,	"lwin" },
{ 86	,"rwin" },
{ 87,	"app" },
{ 88,	"up" },
{ 89	,"left" },
{ 90,	"down" },
{ 91,	"right" },
{ 92,	"f1" },
{ 93,	"f2" },
{ 94,	"f3" },
{ 95,	"f4" },
{ 96,	"f5" },
{ 97,	"f6" },
{ 98,	"f7" },
{ 99, "f8" },
{ 100, "f9" },
{ 101, "f10" },
{ 102, "f11" },
{ 103, "f12" },
{ 104, "capslocktoggle" },
{ 105, "numlocktoggle" },
{ 106, "scrolllocktoggle" },
{ 107, "left mouse" },
{ 108, "right mouse" },
{ 109, "middle mouse" },
{ 110, "mouse4" },
{ 111, "mouse5" },
{ 112, "mwheel up" },
{ 113, "mwheel down" },
};


void gui::initialize( ) {
	ImGui::CreateContext( );
	ImGui_ImplDX9_Init( FindWindow( "Valve001", 0 ), **reinterpret_cast< IDirect3DDevice9*** >( UTILS::FindSignature( "shaderapidx9.dll", "88 56 10" ) + 17 ) );
	ImGui::StyleColorsDark( );

	ImVec4 *colors = ImGui::GetStyle( ).Colors;

	ImGuiStyle& style = ImGui::GetStyle( );

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2( 8, 8 );
	style.WindowMinSize = ImVec2( 32, 32 );
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2( 0.5f, 0.5f );
	style.ChildRounding = 0.0f;
	style.FramePadding = ImVec2( 4, 3 );
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2( 8, 4 );
	style.ItemInnerSpacing = ImVec2( 4, 4 );
	style.TouchExtraPadding = ImVec2( 0, 0 );
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2( 0.5f, 0.5f );
	style.DisplayWindowPadding = ImVec2( 22, 22 );
	style.DisplaySafeAreaPadding = ImVec2( 4, 4 );
	style.AntiAliasedLines = true;
	style.AntiAliasedFill = true;
	style.CurveTessellationTol = 1.25f;

	ColorVar mainColor = ImColor( 54, 54, 54, 255 );
	ColorVar bodyColor = ImColor( 24, 24, 24, 240 );
	ColorVar fontColor = ImColor( 255, 255, 255, 255 );

	ImVec4 mainColorHovered = ImVec4( mainColor.Color( ).Value.x + 0.1f, mainColor.Color( ).Value.y + 0.1f, mainColor.Color( ).Value.z + 0.1f, mainColor.Color( ).Value.w );
	ImVec4 mainColorActive = ImVec4( mainColor.Color( ).Value.x + 0.2f, mainColor.Color( ).Value.y + 0.2f, mainColor.Color( ).Value.z + 0.2f, mainColor.Color( ).Value.w );
	ImVec4 menubarColor = ImVec4( bodyColor.Color( ).Value.x, bodyColor.Color( ).Value.y, bodyColor.Color( ).Value.z, bodyColor.Color( ).Value.w - 0.8f );
	ImVec4 frameBgColor = ImVec4( bodyColor.Color( ).Value.x, bodyColor.Color( ).Value.y, bodyColor.Color( ).Value.z, bodyColor.Color( ).Value.w + .1f );
	ImVec4 tooltipBgColor = ImVec4( bodyColor.Color( ).Value.x, bodyColor.Color( ).Value.y, bodyColor.Color( ).Value.z, bodyColor.Color( ).Value.w + .05f );


	style.Colors [ ImGuiCol_Text ] = fontColor.Color( );
	style.Colors [ ImGuiCol_TextDisabled ] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
	style.Colors [ ImGuiCol_WindowBg ] = bodyColor.Color( );
	style.Colors [ ImGuiCol_ChildWindowBg ] = ImVec4( .0f, .0f, .0f, .0f );
	style.Colors [ ImGuiCol_PopupBg ] = tooltipBgColor;
	style.Colors [ ImGuiCol_Border ] = ImColor( 0, 0, 0, 255 );
	style.Colors [ ImGuiCol_BorderShadow ] = ImVec4( 0.92f, 0.91f, 0.88f, 0.00f );
	style.Colors [ ImGuiCol_FrameBg ] = frameBgColor;
	style.Colors [ ImGuiCol_FrameBgHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_FrameBgActive ] = mainColorActive;
	style.Colors [ ImGuiCol_TitleBg ] = mainColor.Color( );
	style.Colors [ ImGuiCol_TitleBgCollapsed ] = ImVec4( 1.00f, 0.98f, 0.95f, 0.75f );
	style.Colors [ ImGuiCol_TitleBgActive ] = mainColor.Color( );
	style.Colors [ ImGuiCol_MenuBarBg ] = menubarColor;
	style.Colors [ ImGuiCol_ScrollbarBg ] = ImVec4( frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w );
	style.Colors [ ImGuiCol_ScrollbarGrab ] = mainColor.Color( );
	style.Colors [ ImGuiCol_ScrollbarGrabHovered ] = mainColor.Color( );
	style.Colors [ ImGuiCol_ScrollbarGrabActive ] = mainColor.Color( );
	style.Colors [ ImGuiCol_CheckMark ] = mainColor.Color( );
	style.Colors [ ImGuiCol_SliderGrab ] = mainColor.Color( );
	style.Colors [ ImGuiCol_SliderGrabActive ] = mainColorActive;
	style.Colors [ ImGuiCol_Button ] = mainColor.Color( );
	style.Colors [ ImGuiCol_ButtonHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_ButtonActive ] = mainColorActive;
	style.Colors [ ImGuiCol_Header ] = mainColor.Color( );
	style.Colors [ ImGuiCol_HeaderHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_HeaderActive ] = mainColorActive;
	style.Colors [ ImGuiCol_Column ] = mainColor.Color( );
	style.Colors [ ImGuiCol_ColumnHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_ColumnActive ] = mainColorActive;
	style.Colors [ ImGuiCol_ResizeGrip ] = ImColor( 0, 0, 0, 0 );
	style.Colors [ ImGuiCol_ResizeGripHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_ResizeGripActive ] = mainColorActive;
	style.Colors [ ImGuiCol_PlotLines ] = mainColor.Color( );
	style.Colors [ ImGuiCol_PlotLinesHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_PlotHistogram ] = mainColor.Color( );
	style.Colors [ ImGuiCol_PlotHistogramHovered ] = mainColorHovered;
	style.Colors [ ImGuiCol_TextSelectedBg ] = mainColorHovered;
	style.Colors [ ImGuiCol_ModalWindowDarkening ] = ImVec4( 0.0f, 0.0f, 0.0f, 0.75f );
	style.Colors [ ImGuiCol_WindowBorder ] = ImColor( 0, 125, 255, 0 );
}

void gui::shutdown( ) {
	ImGui_ImplDX9_Shutdown( );
}

void gui::create_objects( ) {
	ImGui_ImplDX9_CreateDeviceObjects( );
}

void gui::destroy_objects( ) {
	ImGui_ImplDX9_InvalidateDeviceObjects( );
}

void gui::begin( ) {
	ImGui::GetIO( ).MouseDrawCursor = Vars::options.menu_open;
	ImGui_ImplDX9_NewFrame( );
}

const char* tabs [ ] = { "Aimbot", "Visuals", "Misc", "Anti-Aim", "Configs" };
const char* acc_mode [ ] = { "Head", "Body", "All" };
const char* chams_mode [ ] = { "None", "Visible", "XQZ", "Visible Flat", "XQZ Flat" };
const char* res_type [ ] = { "Default", "Experimental", "Beta(test purposes)" };
const char* aa_mode [ ] = { "None", "Backwards", "Sideways", "Backjitter", "Legit AA", "Slow-Spin" };
const char* aa_mode_fake [ ] = { "None", "Backwards", "Sideways", "Backjitter", "Slow-spin", "Slide-Jitter" };
const char* aa_mode_pitch [ ] = { "None", "Down", "Up", "Fake zero(up)", "Fake zero(down)", "Nospread" };
const char* configs [ ] = { "Default", "Legit", "Autos", "Scouts", "Pistols", "AWP's", "SMG's", "Testing", "Main", "Nospread" };
const char* clantags [ ] = { "None", "bodyaim", "lonely" };
const char* box_style [ ] = { "Disabled", "Enabled" };
std::string config;
const char* arm_types [ ] = { "None", "Wireframe", "Chams" };
const char* glow_style [ ] = { "Regular", "Pulse", "Outline", "Outline pulse" };
const char* visual_sub_tabs [ ] = { "Visual", "Misc", "Effects", "Local", "Colors" };
const char* hitsounds [ ] = { "None", "Metallic", "Default", "Both" };
const char* wep_types [ ] = { "Text", "Icons" };
const char* key_types [ ] = { "Thirdperson", "Manual AA Left", "Manual AA Right", "Fakewalk"/*, "Override"*/ };

void KeyBindButton( ButtonCode_t* key ) {
	bool clicked = false;

	std::string text = ctx::input_system->ButtonCodeToString( *key );
	std::string unique_id = std::to_string( ( int ) key );

	if ( *key <= BUTTON_CODE_NONE )
		text = "_";

	if ( GLOBAL::input_shouldListen && GLOBAL::input_receivedKeyval == key ) {
		clicked = true;
		text = " ";
	}
	text += "]";

	ImGui::LabelClick( "[", "[", text.c_str( ), &clicked, unique_id.c_str( ) );

	if ( clicked ) {
		GLOBAL::input_shouldListen = true;
		GLOBAL::input_receivedKeyval = key;
	}

	if ( *key == KEY_ESCAPE ) {
		*key = BUTTON_CODE_NONE;
	}
}

void aimbot( ) {
	ImGui::Checkbox( "Enable aimbot", &Vars::options.aim_bool );
	ImGui::Checkbox( "Automatic fire", &Vars::options.aim_autofire );
	Vars::options.auto_Revolver = Vars::options.aim_autofire;
	ImGui::SliderFloat( "Hitchance", &Vars::options.chance_val, 0, 100, "%.0f" );
	ImGui::SliderFloat( "Minimum damage", &Vars::options.damage_val, 1, 100, "%.0f" );
	ImGui::Combo( "Hitbox scanning", &Vars::options.acc_type, acc_mode, IM_ARRAYSIZE( acc_mode ), 3 );
	ImGui::Checkbox( "Resolve entity", &Vars::options.aim_resolver );
	ImGui::Combo( "Resolver type", &Vars::options.res_type, res_type, IM_ARRAYSIZE( res_type ), 2 );
	ImGui::Checkbox( "Fake-lag fix", &Vars::options.fakefix_bool );
	//ImGui::Checkbox( "Fake latency", &Vars::options.fakelatency ); <----- works fine, need to fix crashes and make aimbot shoot at it.
//	ImGui::SliderFloat( "Fake latency amount", &Vars::options.fakelatency_amt, 0.0f, 1000.f, "%.0f" );

}

void visuals( ) {

	static int page = 0;
	if ( ImGui::Button( "Visuals", ImVec2( ImGui::GetWindowWidth( ) / 5 - 10, 20 ) ) )page = 0;
	ImGui::SameLine( );
	if ( ImGui::Button( "Misc", ImVec2( ImGui::GetWindowWidth( ) / 5 - 10, 20 ) ) )page = 1;
	ImGui::SameLine( );
	if ( ImGui::Button( "Effects", ImVec2( ImGui::GetWindowWidth( ) / 5 - 10, 20 ) ) )page = 2;
	ImGui::SameLine( );
	if ( ImGui::Button( "Local", ImVec2( ImGui::GetWindowWidth( ) / 5 - 10, 20 ) ) )page = 3;
	ImGui::SameLine( );
	if ( ImGui::Button( "Hotkeys", ImVec2( ImGui::GetWindowWidth( ) / 5 - 10, 20 ) ) )page = 4;


	switch ( page ) {
		case 0:
			ImGui::Checkbox( "Enable visuals", &Vars::options.esp_bool );
			ImGui::Checkbox( "Box ESP", &Vars::options.box_bool );
			ImGui::Checkbox( "Entity name", &Vars::options.name_bool );
			ImGui::Checkbox( "Distance", &Vars::options.distance_bool );
			ImGui::Checkbox( "Flags", &Vars::options.flags_bool );
			ImGui::Checkbox( "Zoom indicator", &Vars::options.zoom );
			ImGui::Checkbox( "Money", &Vars::options.money );
			ImGui::Checkbox( "Fake angle indicator", &Vars::options.fake );
			ImGui::Checkbox( "Last LBY update", &Vars::options.lbyupdate );
			ImGui::Checkbox( "Velocity", &Vars::options.velocity );
			ImGui::Checkbox( "Weapon", &Vars::options.weap_bool );
			if ( Vars::options.weap_bool )
				ImGui::Combo( "Type", &Vars::options.wep_type, wep_types, IM_ARRAYSIZE( wep_types ) );
			ImGui::Checkbox( "Damage indicators", &Vars::options.dmg_bool );
			ImGui::Checkbox( "Health bar", &Vars::options.health_bool );
			ImGui::Checkbox( "Resolve mode", &Vars::options.resolve );
			ImGui::SliderFloat( "Viewmodel FOV", &Vars::options.vis_vfov, 68, 120, "%.0f" );
			ImGui::Combo( "Colored model", &Vars::options.chams_type, chams_mode, IM_ARRAYSIZE( chams_mode ) );

		/*	ImGui::Checkbox( "Entity glow", &Vars::options.glow );
			if ( Vars::options.glow ) {
				ImGui::Combo( "Glow style", &Vars::options.glow_style, glow_style, IM_ARRAYSIZE( glow_style ) );
				ImGui::Combo( "Local low style", &Vars::options.glow_style_local, glow_style, IM_ARRAYSIZE( glow_style ) );
			}*/
			break;
		case 1:
			ImGui::Checkbox( "Hitmarker", &Vars::options.esp_hitmarker );
			ImGui::Combo( "Hitsound", &Vars::options.hitsound, hitsounds, IM_ARRAYSIZE( hitsounds ) );
			ImGui::Checkbox( "Out of POV circles", &Vars::options.offscreen );
			if ( Vars::options.offscreen )
				ImGui::SliderFloat( "Circles size", &Vars::options.offscreenballsize, 1, 10, "%.1f" );

			break;
		case 2:
			ImGui::Checkbox( "Night mode", &Vars::options.night_bool );
			ImGui::Checkbox( "Bullet beams", &Vars::options.beam_bool );
			if ( Vars::options.beam_bool ) {
				ImGui::Checkbox( "Local player beams", &Vars::options.local_beams );
				ImGui::Checkbox( "Enemy beams", &Vars::options.enemy_beams );
				ImGui::Checkbox( "Teammate beams", &Vars::options.team_beams );
				ImGui::SliderFloat( "Beam time", &Vars::options.beam_time, 0, 10, "%.0f" );
			}
			ImGui::Checkbox( "No visual recoil", &Vars::options.novisual_recoil );
			ImGui::Checkbox( "Modulate props", &Vars::options.asus_props );
			ImGui::Checkbox( "Thirdperson", &Vars::options.tp_bool );

			if ( Vars::options.tp_bool )
				ImGui::SliderFloat( "Thirdperson distance", &Vars::options.tp_distance, 0, 200, "%.0f" );
			ImGui::Checkbox( "Show bullet impacts", &Vars::options.show_impact );
			break;
		case 3:
			ImGui::Checkbox( "Local ESP", &Vars::options.box_bool_local );
			ImGui::Checkbox( "Local chams", &Vars::options.chams_local );
			break;
		case 4:
			static int cur_key = 0;
			static int type = 0;
			ImGui::ListBox( "##keys", &cur_key, key_types, IM_ARRAYSIZE( key_types ), 15 );

			ImGui::SameLine( );

			ImGui::BeginChild( "keys", ImVec2( 0, 300 ) );
			static char FilterKeys [ 32 ];
			ImGui::PushItemWidth( -1 );
			ImGui::InputText( "##Hotkey", FilterKeys, 32 );
			ImGui::PopItemWidth( );
			ImGui::ListBoxHeader( "##keys", ImVec2( -1 , 236 ) );
			switch ( cur_key ) {
				case 0:
					for ( auto it : ButtonNamesA ) {
						if ( !( std::string( it.second ).find( std::string( FilterKeys ) ) != std::string::npos ) )
							continue;

						const bool item_selected = ( it.first == Vars::options.tp_key );

						ImGui::PushID( it.first );
						if ( ImGui::Selectable( it.second, item_selected ) )
							Vars::options.tp_key = it.first;

						ImGui::PopID( );
					}
					break;
				case 1:
					for ( auto it : ButtonNamesA ) {
						if ( !( std::string( it.second ).find( std::string( FilterKeys ) ) != std::string::npos ) )
							continue;

						const bool item_selected = ( it.first == Vars::options.aa_left );

						ImGui::PushID( it.first );
						if ( ImGui::Selectable( it.second, item_selected ) )
							Vars::options.aa_left = it.first;

						ImGui::PopID( );
					}
					break;
				case 2:
					for ( auto it : ButtonNamesA ) {
						if ( !( std::string( it.second ).find( std::string( FilterKeys ) ) != std::string::npos ) )
							continue;

						const bool item_selected = ( it.first == Vars::options.aa_right );

						ImGui::PushID( it.first );
						if ( ImGui::Selectable( it.second, item_selected ) )
							Vars::options.aa_right = it.first;

						ImGui::PopID( );
					}
					break;
				case 3:
					for ( auto it : ButtonNamesA ) {
						if ( !( std::string( it.second ).find( std::string( FilterKeys ) ) != std::string::npos ) )
							continue;

						const bool item_selected = ( it.first == Vars::options.fakewalk );

						ImGui::PushID( it.first );
						if ( ImGui::Selectable( it.second, item_selected ) )
							Vars::options.fakewalk = it.first;

						ImGui::PopID( );
					}
					break;
				case 4:
					for ( auto it : ButtonNamesA ) {
						if ( !( std::string( it.second ).find( std::string( FilterKeys ) ) != std::string::npos ) )
							continue;

						const bool item_selected = ( it.first == Vars::options.override_key );

						ImGui::PushID( it.first );
						if ( ImGui::Selectable( it.second, item_selected ) )
							Vars::options.override_key = it.first;

						ImGui::PopID( );
					}
					break;
			}
			ImGui::ListBoxFooter( );

			ImGui::EndChild( );

			break;
	}
}

void misc( ) {
	static bool lol;
	ImGui::Checkbox( "Enable miscellaneous", &Vars::options.misc_bool );
	ImGui::Checkbox( "Auto jump", &Vars::options.bhop_bool );
	ImGui::Checkbox( "Auto strafer", &Vars::options.strafe_bool );
	ImGui::Checkbox( "Fakelag", &Vars::options.lag_bool );
	ImGui::Checkbox( "Fakelag while jumping", &Vars::options.stop_bool );
	ImGui::Checkbox( "Fakelag while moving", &lol );
	if ( Vars::options.stop_bool )
		ImGui::SliderFloat( "Value while jumping", &Vars::options.jump_lag, 1, 15, "%.0f" );
	if ( lol )
		ImGui::SliderFloat( "Value while moving", &Vars::options.move_lag, 1, 15, "%.0f" );

	ImGui::Checkbox( "Clantag spammer", &Vars::options.clantag );

	ImGui::PushItemWidth( 112 );
	ImGui::Combo( "##Configs", &Vars::options.config_sel, configs, IM_ARRAYSIZE( configs ) );
	switch ( Vars::options.config_sel ) {
		case 0: config = "Default"; break;
		case 1: config = "Legit"; break;
		case 2: config = "Auto - HvH"; break;
		case 3: config = "Scout - HvH"; break;
		case 4: config = "Pistol - HvH"; break;
		case 5: config = "AWP - HvH"; break;
		case 6: config = "SMG's - HvH"; break;
		case 7: config = "Testing - HvH"; break;
		case 8: config = "Main - HvH"; break;
		case 9: config = "Nospread - HvH"; break;
	}
	ImGui::PopItemWidth( );

	if ( ImGui::Button( "Load", ImVec2( 50, 20 ) ) )
		Vars::options.Load( config );

	ImGui::SameLine( );

	if ( ImGui::Button( "Save", ImVec2( 50, 20 ) ) )
		Vars::options.Save( config );

}

void antiaim( ) {
	ImGui::Checkbox( "Enable anti-aim", &Vars::options.aa_bool );
	ImGui::Combo( "Anti-aim pitch", &Vars::options.aa_type_x, aa_mode_pitch, IM_ARRAYSIZE(aa_mode_pitch));
	ImGui::Combo( "Anti-aim real", &Vars::options.aa_type, aa_mode, IM_ARRAYSIZE( aa_mode ) );
	ImGui::Combo( "Anti-aim fake", &Vars::options.aa_type_fake, aa_mode_fake, IM_ARRAYSIZE( aa_mode_fake ) );

	ImGui::Checkbox("LBY Breaker", &Vars::options.lby_breaker_bool);
	if (Vars::options.lby_breaker_bool)
	{
		ImGui::SliderFloat("##lbydelta", &Vars::options.delta_val, 0.f, 180.f, "LBY Delta: %.f");
	}

}

void gui::render( ) {
	Vars::options.localesp = true;
	Vars::options.info_bool = true; 
	static int page = 0;

	if ( Vars::options.menu_open ) { 
		ImGui::SetNextWindowSize( ImVec2( 500, 250 ), ImGuiCond_FirstUseEver );
		ImGui::Begin( "gamesneeze", &Vars::options.menu_open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar );
		ImVec2 p = ImGui::GetCursorScreenPos( );
		ImGui::GetWindowDrawList( )->AddRectFilledMultiColor( ImVec2( p.x - 5, p.y - 8 ), ImVec2( p.x + ImGui::GetWindowWidth( ) / 2 - 5, p.y - 5 ), ImColor( 55, 177, 218 ), ImColor( 201, 84, 192 ), ImColor( 201, 84, 192 ), ImColor( 55, 177, 218 ) );
		ImGui::GetWindowDrawList( )->AddRectFilledMultiColor( ImVec2( p.x + ImGui::GetWindowWidth( ) / 2 - 5, p.y - 8 ), ImVec2( p.x + ImGui::GetWindowWidth( ) - 5, p.y - 5 ), ImColor( 201, 84, 192 ), ImColor( 204, 227, 54 ), ImColor( 204, 227, 54 ), ImColor( 201, 84, 192 ) );
		long currentTime_ms = std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now( ).time_since_epoch( ) ).count( );
		static long timeStamp = currentTime_ms;

		timeStamp = currentTime_ms + .5;

		const char* change = "";
		switch ( timeStamp % 4 ) {
			case 0:change = "|"; break;
			case 1:change = "/"; break;
			case 2:change = "-"; break;
			case 3:change = "\\"; break;
		}
		ImGui::Text( "gamesneeze" );
		ImGui::SameLine( );
		ImGui::Text( change );
		ImGui::BeginChild( "tabs", ImVec2( 0, 20 ), false );
		if ( ImGui::Button( "Aimbot", ImVec2( ImGui::GetWindowWidth( ) / 4 - 6, 20 ) ) ) page = 0;
		ImGui::SameLine( );
		if ( ImGui::Button( "Visuals", ImVec2( ImGui::GetWindowWidth( ) / 4 - 6, 20 ) ) ) page = 1;
		ImGui::SameLine( );
		if ( ImGui::Button( "Misc", ImVec2( ImGui::GetWindowWidth( ) / 4 - 6, 20 ) ) ) page = 2;
		ImGui::SameLine( );
		if ( ImGui::Button( "Anti-Aim", ImVec2( ImGui::GetWindowWidth( ) / 4 - 6, 20 ) ) ) page = 3;
		ImGui::EndChild( );

		ImGui::BeginChild( "main", ImVec2( 0, 0 ), true, ImGuiWindowFlags_NoScrollbar );

		ImGui::PushItemWidth( 175 );
		switch ( page ) {
			case 0:
				aimbot( );
				break;
			case 1:
				visuals( );
				break;
			case 2:
				misc( );
				break;
			case 3:
				antiaim( );
				break;
		}
		ImGui::PopItemWidth( );

		ImGui::EndChild( );

		ImGui::End( );
	}
}

void gui::end( ) {
	ImGui::Render( );
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
}
