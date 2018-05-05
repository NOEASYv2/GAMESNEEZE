#pragma once

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

namespace sdk
{
	class CBaseEntity;
}

namespace UTILS
{
	struct BulletImpact_t
	{
		float			flImpactTime;
		Vector			vecImpactPos;
		CColor			color;
		sdk::CBaseEntity*	pPlayer;

		__forceinline BulletImpact_t()
		{
			vecImpactPos = { 0.0f, 0.0f, 0.0f };
			flImpactTime = 0.0f;
			color = WHITE;
			pPlayer = nullptr;
		}

		__forceinline BulletImpact_t(sdk::CBaseEntity* player, Vector pos, float time, CColor col = WHITE)
		{
			pPlayer = player;
			flImpactTime = time;
			vecImpactPos = pos;
			color = col;
		}
	};
}