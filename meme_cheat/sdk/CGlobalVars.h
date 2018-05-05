#pragma once

namespace sdk
{
	class CGlobalVars
	{
	public:
		float	realtime; // 0x4
		int		framecount; // 0x8
		float	absoluteframetime; // 0xC
		float	absoluteframestarttimestddev; // 0x10
		float	curtime; // 0x14
		float	frametime; // 0x18
		int		maxclients; // 0x1B
		int		tickcount; //0x1F
		float	interval_per_tick;
		float	interpolation_amount;
		int		simTicksThisFrame;
		int		network_protocol;
		char	pad00[14];
		CUserCmd* user_cmd;
	};
}

