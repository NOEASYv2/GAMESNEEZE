#pragma once

namespace sdk
{
	struct player_info_t
	{
		char __pad0[0x8];

		int xuidlow;
		int xuidhigh;

		char name[128];
		int userid;
		char guid[33];

		char __pad1[0x17B];
	};
}