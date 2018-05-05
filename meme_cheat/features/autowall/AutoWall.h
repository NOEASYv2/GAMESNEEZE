#pragma once

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
	struct Weapon_Info;
	class CSWeaponInfo;
}



class CAutoWall
{
private:
	struct Autowall_Return_Info
	{
		int damage;
		int hitgroup;
		Vector end;
		sdk::CBaseEntity* hit_entity;
	};
	struct Autowall_Info {
		Vector start;
		Vector end;
		Vector current_position;

		Vector direction;

		sdk::ITraceFilter* filter;
		sdk::trace_t enter_trace;

		float current_damage;
		int penetration_count;
	};
	struct twoja_stara {
		twoja_stara( const Vector &eye_pos )
			: src( eye_pos ) { }

		Vector				  src;
		sdk::trace_t          enter_trace;
		Vector				  direction;
		sdk::CTraceFilter     filter;
		float				  trace_length;
		float				  trace_length_remaining;
		float				  current_damage;
		int					  penetrate_count;
	};
	void UTIL_TraceLine(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, sdk::CBaseEntity *ignore, int collisionGroup, sdk::trace_t *ptr)
	{
		sdk::Ray_t ray;

		sdk::CTraceFilter filter;
		filter.pSkip1 = ignore;

		ray.Init(vecAbsStart, vecAbsEnd);

		ctx::trace->TraceRay(ray, mask, &filter, ptr);
	}
	void UTIL_TraceLineSig(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const sdk::CBaseEntity *ignore, int collisionGroup, sdk::trace_t *ptr)
	{
		typedef int(__fastcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const sdk::CBaseEntity*, int, sdk::trace_t*);
		static UTIL_TraceLine_t TraceLine = (UTIL_TraceLine_t)UTILS::FindSignature("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52");
		TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
	}
	void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, sdk::ITraceFilter* filter, sdk::trace_t* tr)
	{
		static DWORD dwAddress = UTILS::FindPattern("client.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");
		if (!dwAddress)
			return;

		_asm
		{
			MOV        EAX, filter
			LEA        ECX, tr
			PUSH    ECX
			PUSH    EAX
			PUSH    mask
			LEA        EDX, vecAbsEnd
			LEA        ECX, vecAbsStart
			CALL    dwAddress
			ADD        ESP, 0xC
		}
	}
public:
	Autowall_Return_Info CalculateDamage(Vector start, Vector end, sdk::CBaseEntity * from_entity, sdk::CBaseEntity * to_entity);
	bool CanPenetrate(sdk::CBaseEntity * attacker, Autowall_Info & info, sdk::CSWeaponInfo * weapon_data);
	void ScaleDamage(int hitgroup, sdk::CBaseEntity * player, float weapon_armor_ratio, float & current_damage);
	bool IsArmored(sdk::CBaseEntity* Entity, int ArmorValue, int Hitgroup);
	bool HandleBulletPenetration( Autowall_Info & data );
	bool FloatingPointIsVisible(sdk::CBaseEntity * local_player, const Vector & point);
	bool HandleBulletPenetration( twoja_stara & data );
	bool xdddd( sdk::CSWeaponInfo * wpn_data, twoja_stara & data );
	bool CanWallbang( int &dmg );
	bool HandleBulletPenetration( sdk::CSWeaponInfo * wpn_data, twoja_stara & data );
	bool CanWallbang( float & dmg );
};

extern CAutoWall* autowall;