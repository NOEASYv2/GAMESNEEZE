#pragma once

namespace sdk
{
	/* every csgo activity */
	enum CSGO_ACTS {
		ACT_CSGO_DEFUSE = 958,
		ACT_CSGO_DEFUSE_WITH_KIT = 959,
		ACT_CSGO_FLASHBANG_REACTION = 960,
		ACT_CSGO_FIRE_PRIMARY = 961, 
		ACT_CSGO_FIRE_PRIMARY_OPT_1 = 962,
		ACT_CSGO_FIRE_PRIMARY_OPT_2 = 963,
		ACT_CSGO_FIRE_SECONDARY = 964, // shooting secondary
		ACT_CSGO_RELOAD = 967,
		ACT_CSGO_RELOAD_START = 968,
		ACT_CSGO_RELOAD_LOOP = 969,
		ACT_CSGO_RELOAD_END = 970,
		ACT_CSGO_PLANT_BOMB = 978,
		ACT_CSGO_IDLE_TURN_BALANCEADJUST = 979,
		ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING = 980,
	};

	class CAnimationLayer
	{
	public:
		char  pad_0000[20];
		uint32_t m_nOrder; //0x0014
		uint32_t m_nSequence; //0x0018
		float_t m_flPrevCycle; //0x001C
		float_t m_flWeight; //0x0020
		float_t m_flWeightDeltaRate; //0x0024
		float_t m_flPlaybackRate; //0x0028
		float_t m_flCycle; //0x002C
		void *m_pOwner; //0x0030 // player's thisptr
		char  pad_0038[4]; //0x0034
	}; //Size: 0x0038
}