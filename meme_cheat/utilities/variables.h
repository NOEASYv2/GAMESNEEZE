#pragma once

#include "../sdk/CInputSystem.h"

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/

namespace GLOBAL
{
	extern HWND csgo_hwnd;

	extern bool should_send_packet;
	extern bool is_fakewalking;
	extern int choke_amount;
	extern int resolve_mode;
	extern const char* change;
	extern std::string current_resolve;
	extern float yaw;
	extern float alpha;
	extern bool bFakewalking;
	extern Vector vecUnpredictedVel;
	extern bool bShouldChoke;
	extern bool is_autowalling;
	extern int nChockedTicks;
	extern std::string current_cycle;
	extern int shots_missed;
	extern float stored_lby [65];
	extern int MissedShots;
	extern bool valid_tick [ 65 ];
	extern bool suppressing_979;
	extern int antiaim_state;
	extern int shots_hit;
	extern int shots_fired;
	extern int current_activity;
	extern bool input_shouldListen;
	extern ButtonCode_t* input_receivedKeyval;

	extern Vector real_angles;
	extern Vector fake_angles;
}
namespace FONTS
{
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_font;
	extern unsigned int menu_window_font;
	extern unsigned int visuals_weapon_icon_font;
	extern unsigned int numpad_menu_font;
	extern unsigned int visuals_esp_font;
	extern unsigned int visuals_weapon_font;
	extern unsigned int visuals_xhair_font;
	extern unsigned int visuals_side_font;
	extern unsigned int visuals_name_font;
	extern unsigned int visuals_lby_font;
	
	bool ShouldReloadFonts();
	void InitFonts();
}
namespace Vars
{
	class CSettings {
	public:
		// returns true/false whether the function succeeds, usually returns false if file doesn't exist
		bool Save( std::string file_name );
		bool Load( std::string file_name );

		void CreateConfig( ); // creates a blank config

		std::vector<std::string> GetConfigs( );

		bool bhop_bool;
		bool misc_clientlagger;
		bool strafe_bool;
		bool arms;
		bool esp_bool;
		int wep_type;
		int chams_type;
		CColor arms_col = CColor(255,255,255,255);
		bool lbyupdate;
		bool money;
		bool velocity;
		bool fake;
		bool zoom;
		bool chams_local;
		bool localesp;
		bool glow_local;
		int hitsound;
		int res_type;
		int xhair_type;
		bool fakelatency;
		float fakelatency_amt;
		float asus_props_o = 100.f;
		bool flatency_bool;
		bool resolve;
		float flatency_amt;
		bool current_cycle;
		bool current_cycle_local;
		bool show_spread;
		bool speclist;
		bool draw_bt;
		bool local_box;
		bool fake_angs;
		bool fake_angs_lby;
		float fake_angle;
		float spread_alpha = 100;
		bool show_impact;
		CColor spread_color;
		bool tp_bool;

		int tp_key;
		int aa_right;
		int aa_left;
		int fakewalk;
		int override_key;

		bool esp_hitmarker;
		bool aim_bool;
		bool aim_silent;
		bool aim_autofire;
		bool menu_open;
		bool offscreen;
		CColor offscreencolor;
		CColor fake_angsclr;
		CColor arrow_col;
		CColor beam_col;
		CColor beam_col_e;
		CColor beam_col_t;
		bool local_beams;
		bool team_beams;
		bool enemy_beams;
		float beam_time = 3.f;
		float offscreenballsize;
		float vis_vfov = 68.f;
		float chams_alpha = 100;
		float chams_alpha_local = 100;
		float chams_alpha_fake = 100;
		float tp_distance = 75.f;
		int aim_type;
		float ct_speed = 0.35f;
		bool aim_resolver;
		bool auto_Revolver;
		bool aa_autodir;
		float edge_distance = 25.f;
		bool aa_bool;
		int aa_type;
		int aa_type_fake;
		int aa_type_x;
		int acc_type;
		bool clantag;
		bool up_bool;
		bool misc_bool;
		int config_sel;
		int ct;
		bool beam_bool;
		bool dmg_bool;
		CColor dmg_col;
		bool stop_bool;
		bool custom_clantag;
		bool novisual_recoil;
		bool night_bool;
		bool asus_walls;
		bool asus_props;
		bool glow;
		float asus_walls_o = 0.199999995f;
		CColor glowColor;
		int glow_style;
		int glow_style_local;
		bool box_bool;
		bool box_bool_local;
		bool name_bool_local;
		bool flags_bool_local;
		bool zoom_local;
		bool money_local;
		bool lbyupdate_local;
		bool velocity_local;
		bool weap_bool_local;
		bool health_bool_local;
		bool name_bool;
		bool distance_bool;
		bool flags_bool;
		bool weap_bool;
		bool health_bool;
		bool info_bool;
		bool back_bool;
		bool lag_bool;
		int box_type;
		bool reverse_bool;
		bool multi_bool;
		bool fakefix_bool;
		bool engine_pred;

		bool lby_breaker_bool;

		float stand_lag;
		float move_lag;
		float jump_lag;

		bool debug_bool;

		CColor vmodel_col;
		CColor imodel_col;
		CColor lmodel_col;
		CColor glowColor_l;
		CColor box_col;
		CColor menu_col = CColor( 117, 160, 13 );

		float chance_val;
		float damage_val;
		float delta_val;
		float point_val;
		float body_val;

		int flip_side = 0;

		int aa_side;

	private:
	}; extern CSettings options;
}

/*extern bool bhop_bool;
extern bool strafe_bool;
extern bool esp_bool;
extern int chams_type;
extern int xhair_type;
extern bool tp_bool;
extern bool aim_bool;
extern int aim_type;
extern bool aa_bool;
extern int aa_type;
extern int acc_type;
extern bool up_bool;
extern bool misc_bool;

extern float chance_val;
extern float damage_val;
extern float delta_val;

extern bool flip_bool;
extern int aa_side;
extern bool in_tp;*/

extern bool using_fake_angles[65];

extern bool in_tp;
extern bool fake_walk;

extern int resolve_type[65];

extern int shots_fired[65];
extern int shots_hit[65];
extern int shots_missed[65];

extern float tick_to_back[65];
extern float lby_to_back[65];
extern bool backtrack_tick[65];

extern float lby_delta;
extern float update_time[65];

extern int hitmarker_time;

extern bool menu_hide;

extern int oldest_tick[65];
extern float compensate[65][12];
extern Vector backtrack_hitbox[65][20][12];
extern float backtrack_simtime[65][12];