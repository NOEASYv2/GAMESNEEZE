#include "../../includes.h"

#include "interfaces.h"

#include "../sdk/ISurface.h"
#include "render.h"
#include "../sdk/IEngine.h"


namespace GLOBAL
{
	HWND csgo_hwnd = FindWindow(0, "Counter-Strike: Global Offensive");

	bool should_send_packet;
	bool is_fakewalking;
	int nChockedTicks = 0;
	bool bFakewalking = false;
	const char* change = "";
	float alpha;
	bool input_shouldListen = false;
	ButtonCode_t* input_receivedKeyval;
	bool bShouldChoke = false;
	bool is_autowalling = false;
	Vector vecUnpredictedVel = Vector( 0, 0, 0 );
	int choke_amount;
	float yaw;
	int resolve_mode = 0;/*
						 0: not resolving
						 1: moving
						 2: standing still
						 3: in air
						 4: predicting lby
						 5: backtrack to last lby update
						 6: bruteforcing
						 7: fakelag fixing
						 8: correcting lby
						 9: moving bruteforce
						 6969: static fake dynamic sharklaser is autistic
						 */
	int antiaim_state = 0;/*
						  0: nothing
						  1: breaking lby
						  2: breaking lby & was supressing 979
						  */
	std::string current_resolve;
	bool suppressing_979 = false;
	std::string current_cycle;
	int shots_missed = 0;
	bool valid_tick [ 65 ];
	float stored_lby [ 65 ];
	int MissedShots = 0;
	int shots_hit = 0;
	int current_activity;
	int shots_fired = 0;
	Vector real_angles;
	Vector fake_angles;
}
namespace FONTS
{
	unsigned int menu_tab_font;
	unsigned int menu_checkbox_font;
	unsigned int menu_slider_font;
	unsigned int menu_groupbox_font;
	unsigned int menu_combobox_font;
	unsigned int menu_window_font;
	unsigned int numpad_menu_font;
	unsigned int visuals_esp_font;
	unsigned int visuals_weapon_font;
	unsigned int visuals_xhair_font;
	unsigned int visuals_side_font;
	unsigned int visuals_name_font;
	unsigned int visuals_lby_font;
	unsigned int visuals_weapon_icon_font;

	bool ShouldReloadFonts()
	{
		static int old_width, old_height;
		int width, height;
		ctx::engine->GetScreenSize(width, height);

		if (width != old_width || height != old_height)
		{
			old_width = width;
			old_height = height;
			return true;
		}
		return false;
	}
	void InitFonts()
	{
		menu_tab_font = RENDER::CreateF("Skeetfont", 18, 550, 0, 0, NULL);
		menu_checkbox_font = RENDER::CreateF("Verdana", 16, 550, 0, 0, NULL);
		menu_slider_font = RENDER::CreateF("Verdana", 14, 550, 0, 0, NULL);
		menu_groupbox_font = RENDER::CreateF("Verdana", 14, 550, 0, 0, sdk::FONTFLAG_SYMBOL );
		menu_combobox_font = RENDER::CreateF("Verdana", 14, 700, 0, 0, NULL);
		menu_window_font = RENDER::CreateF("Tahoma", 12, 500, 0, 0, sdk::FONTFLAG_DROPSHADOW);
		numpad_menu_font = RENDER::CreateF("Tahoma", 13, 600, 0, 0, sdk::FONTFLAG_OUTLINE);
		visuals_esp_font = RENDER::CreateF("Smallest Pixel-7", 11, 400, 0, 0, sdk::FONTFLAG_OUTLINE);
		visuals_weapon_font = RENDER::CreateF( "Undefeated", 16, 100, 0, 0, sdk::FONTFLAG_ANTIALIAS );
		visuals_xhair_font = RENDER::CreateF("Tahoma", 18, 600, 0, 0, sdk::FONTFLAG_OUTLINE);
		visuals_side_font = RENDER::CreateF("Tahoma", 46, 900, 0, 0, sdk::FONTFLAG_ANTIALIAS);
		visuals_name_font = RENDER::CreateF("Tahoma", 11, 550, 0, 0, sdk::FONTFLAG_DROPSHADOW | sdk::FONTFLAG_ANTIALIAS );
		visuals_lby_font = RENDER::CreateF("Verdana", 32, 650, 0, 0, sdk::FONTFLAG_ANTIALIAS | sdk::FONTFLAG_DROPSHADOW);
		visuals_weapon_icon_font = RENDER::CreateF( "Undefeated", 14, 400, 0, 0, sdk::FONTFLAG_ANTIALIAS | sdk::FONTFLAG_DROPSHADOW );
	}
}
namespace Vars
{
	CSettings options; 

	bool CSettings::Save(std::string file_name)
	{
		std::string file_path = "C:\\gamesneeze\\" + file_name + ".cfg";

		std::fstream file(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		file.close();

		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		const size_t settings_size = sizeof(CSettings);
		for (int i = 0; i < settings_size; i++)
		{
			byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
			for (int x = 0; x < 8; x++)
			{
				file << (int)((current_byte >> x) & 1);
			}
		}

		file.close();

		return true;
	}
	bool CSettings::Load(std::string file_name)
	{
		CreateDirectory("C:\\gamesneeze\\", NULL);

		std::string file_path = "C:\\gamesneeze\\" + file_name + ".cfg";

		std::fstream file;
		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		std::string line;
		while (file)
		{
			std::getline(file, line);

			const size_t settings_size = sizeof(CSettings);
			if (line.size() > settings_size * 8)
			{
				file.close();
				return false;
			}
			for (int i = 0; i < settings_size; i++)
			{
				byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
				for (int x = 0; x < 8; x++)
				{
					if (line[(i * 8) + x] == '1')
						current_byte |= 1 << x;
					else
						current_byte &= ~(1 << x);
				}
				*reinterpret_cast<byte*>(uintptr_t(this) + i) = current_byte;
			}
		}

		file.close();

		return true;
	}
	void CSettings::CreateConfig()
	{
		std::string file_path = "C:\\gamesneeze\\BlankConfig.cfg";

		std::fstream file;
		file.open(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		file.close();
	}

	

	std::vector<std::string> CSettings::GetConfigs()
	{
		std::vector<std::string> configs;

		WIN32_FIND_DATA ffd;
		auto directory = "C:\\gamesneeze\\*";
		auto hFind = FindFirstFile(directory, &ffd);

		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;

				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4); // erase everything but the last 4 letters
				if (end != ".cfg")
					continue;

				file_name.erase(file_name.end() - 4, file_name.end()); // erase the .cfg part
				configs.push_back(file_name);
			}
		}

		return configs;
	}
}