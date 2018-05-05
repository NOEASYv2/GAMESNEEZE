#include "../../includes.h"

#include "../../utilities/interfaces.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/SurfaceData.h"
#include "../../sdk/CTrace.h"

#include "AutoWall.h"

static bool is_autowalling = false;

CAutoWall::Autowall_Return_Info CAutoWall::CalculateDamage(Vector start, Vector end, sdk::CBaseEntity* from_entity, sdk::CBaseEntity* to_entity)
{
	// default values for return info, in case we need to return abruptly
	Autowall_Return_Info return_info;
	return_info.damage = -1;
	return_info.hitgroup = -1;
	return_info.hit_entity = nullptr;

	Autowall_Info autowall_info;
	autowall_info.penetration_count = 4;
	autowall_info.start = start;
	autowall_info.end = end;
	autowall_info.current_position = start;

	// direction 
	MATH::AngleVectors(UTILS::CalcAngle(start, end), &autowall_info.direction);

	// attacking entity
	if (!from_entity)
		from_entity = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());
	if (!from_entity)
		return return_info;

	if (to_entity->GetIsDormant())
		return return_info;

	// setup filters
	auto filter_player = sdk::CTraceFilter();
	filter_player.pSkip1 = from_entity;
	autowall_info.filter = &filter_player;

	// weapon
	auto weapon = reinterpret_cast<sdk::CBaseWeapon*>(ctx::client_ent_list->GetClientEntity(from_entity->GetActiveWeaponIndex()));
	if (!weapon)
		return return_info;

	// weapon data
	auto weapon_info = weapon->get_full_info();
	if (weapon_info == NULL)
		return return_info;

	if (weapon_info == nullptr)
		return return_info;

	if (!weapon_info)
		return return_info;

	autowall_info.current_damage = weapon_info->iDamage;
	const float range = min(weapon_info->flRange, (start - end).Length());
	end = start + (autowall_info.direction * range);


	while (autowall_info.current_damage > 0 && autowall_info.penetration_count > 0)
	{
		UTIL_TraceLine(autowall_info.current_position, end, MASK_SHOT | CONTENTS_GRATE, from_entity, 0, &autowall_info.enter_trace);
		UTIL_ClipTraceToPlayers(autowall_info.current_position, autowall_info.current_position + autowall_info.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, autowall_info.filter, &autowall_info.enter_trace);

		// if reached the end
		if (autowall_info.enter_trace.flFraction == 1.f)
		{
			return_info.damage = autowall_info.current_damage;
			return_info.hitgroup = -1;
			return_info.end = autowall_info.enter_trace.end;
			return_info.hit_entity = nullptr;
		}
		// if hit an entity
		if (autowall_info.enter_trace.hitGroup > 0 && autowall_info.enter_trace.hitGroup <= 7 && autowall_info.enter_trace.m_pEnt)
		{
			// checkles gg
			if ((to_entity && autowall_info.enter_trace.m_pEnt != to_entity) ||
				(autowall_info.enter_trace.m_pEnt->GetTeam() == from_entity->GetTeam()))
			{
				if (!CanPenetrate(from_entity, autowall_info, weapon_info))
					break;

				continue;
			}
			 
			ScaleDamage(autowall_info.enter_trace.hitGroup, autowall_info.enter_trace.m_pEnt, weapon_info->flArmorRatio, autowall_info.current_damage);

			// fill the return info
			return_info.damage = autowall_info.current_damage;
			return_info.hitgroup = autowall_info.enter_trace.hitGroup;
			return_info.end = autowall_info.enter_trace.end;
			return_info.hit_entity = autowall_info.enter_trace.m_pEnt;

			break;
		}

		// break out of the loop retard
		if (!CanPenetrate(from_entity, autowall_info, weapon_info))
			break;
	}
	return return_info; 
}
bool CAutoWall::CanPenetrate(sdk::CBaseEntity* attacker, Autowall_Info &info, sdk::CSWeaponInfo* weapon_data)
{
	typedef bool(__thiscall* HandleBulletPenetrationFn)(sdk::CBaseEntity*, float&, int&, int*, sdk::trace_t*, Vector*, float, float, float, int, int, float, int*, Vector*, float, float, float*);
	static HandleBulletPenetrationFn HBP = reinterpret_cast<HandleBulletPenetrationFn>(UTILS::FindPattern("client.dll",
		(PBYTE)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF8\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x83\xEC\x78\x8B\x53\x14",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

	auto enter_surface_data = ctx::physics_props->GetSurfaceData(info.enter_trace.surface.surfaceProps);
	if (!enter_surface_data)
		return true;

	int use_static_values = 0;
	int material = enter_surface_data->game.material;
	int mask = /*GetWeaponID(local_player) == weapon_taser ? 0x1100 : */0x1002;

	// glass and shit gg
	if (info.enter_trace.m_pEnt && !strcmp("CBreakableSurface",
		info.enter_trace.m_pEnt->GetClientClass()->m_pNetworkName))
		*reinterpret_cast<byte*>(uintptr_t(info.enter_trace.m_pEnt + 0x27C)) = 2;

	GLOBAL::is_autowalling = true;
	bool return_value = !HBP(attacker, weapon_data->flPenetration, material, &use_static_values, &info.enter_trace, &info.direction, 0.f, enter_surface_data->game.flPenetrationModifier, enter_surface_data->game.flDamageModifier, 0, mask, weapon_data->flPenetration, &info.penetration_count, &info.current_position, 0.f, 0.f, &info.current_damage);
	GLOBAL::is_autowalling = false;
	return return_value;
}
void CAutoWall::ScaleDamage(int hitgroup, sdk::CBaseEntity *player, float weapon_armor_ratio, float &current_damage)
{
	int armor = player->GetArmor();

	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		current_damage *= 4.f;
		break;

	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:

		current_damage *= 1.f;
		break;

	case HITGROUP_STOMACH:

		current_damage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:

		current_damage *= 0.75f;
		break;
	}

	if (IsArmored(player, armor, hitgroup))
	{
		float v47 = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = weapon_armor_ratio * 0.5f;

		float new_damage = current_damage * armor_ratio;

		if (((current_damage - (current_damage * armor_ratio)) * (v47 * armor_bonus_ratio)) > armor)
			new_damage = current_damage - (armor / armor_bonus_ratio);

		current_damage = new_damage;
	}
}
bool CAutoWall::IsArmored(sdk::CBaseEntity *player, int armorVal, int hitgroup)
{
	bool res = false;

	if (armorVal > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:

			res = true;
			break;

		case HITGROUP_HEAD:

			res = player->HasHelmet();
			break;

		}
	}

	return res;
}

inline vec_t VectorLength( const Vector& v ) {
	return ( vec_t ) sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}
inline vec_t VectorNormalize( Vector& v ) {
	vec_t l = v.Length( );

	if ( l != 0.0f ) {
		v /= l;
	}
	else {
		v.x = v.y = 0.0f; v.z = 1.0f;
	}

	return l;
}

bool TraceToExit( Vector& end, sdk::trace_t& tr, Vector start, Vector vEnd, sdk::trace_t* trace ) {
	typedef bool( __fastcall* TraceToExitFn )( Vector&, sdk::trace_t&, float, float, float, float, float, float, sdk::trace_t* );
	static DWORD TraceToExit = UTILS::FindPattern( "client.dll", ( BYTE* )"\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75", "xxxxxxxxxx" );

	if ( !TraceToExit )
		return false;

	float start_y = start.y, start_z = start.z, start_x = start.x, dir_y = vEnd.y, dir_x = vEnd.x, dir_z = vEnd.z;

	_asm
	{
		push trace
		push dir_z
		push dir_y
		push dir_x
		push start_z
		push start_y
		push start_x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

bool CAutoWall::HandleBulletPenetration( sdk::CSWeaponInfo *wpn_data, twoja_stara &data ) {
	sdk::surfacedata_t *enter_surface_data = ctx::physics_props->GetSurfaceData( data.enter_trace.surface.surfaceProps );
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;


	data.trace_length += data.enter_trace.flFraction * data.trace_length_remaining;
	data.current_damage *= ( float ) ( pow( wpn_data->flRangeModifier, ( data.trace_length * 0.002 ) ) );

	if ( ( data.trace_length > 3000.f ) || ( enter_surf_penetration_mod < 0.1f ) )
		data.penetrate_count = 0;

	if ( data.penetrate_count <= 0 )
		return false;

	Vector dummy;
	sdk::trace_t trace_exit;
	if ( !TraceToExit( dummy, data.enter_trace, data.enter_trace.end, data.direction, &trace_exit ) )
		return false;

	sdk::surfacedata_t *exit_surface_data = ctx::physics_props->GetSurfaceData( trace_exit.surface.surfaceProps );
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if ( ( ( data.enter_trace.contents & CONTENTS_GRATE ) != 0 ) || ( enter_material == 89 ) || ( enter_material == 71 ) ) {
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else {
		combined_penetration_modifier = ( enter_surf_penetration_mod + exit_surf_penetration_mod ) * 0.5f;
	}

	if ( enter_material == exit_material ) {
		if ( exit_material == 87 || exit_material == 85 )
			combined_penetration_modifier = 3.0f;
		else if ( exit_material == 76 )
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf( 0.f, 1.0f / combined_penetration_modifier );
	float v35 = ( data.current_damage * final_damage_modifier ) + v34 * 3.0f * fmaxf( 0.0f, ( 3.0f / wpn_data->flPenetration ) * 1.25f );
	float thickness = VectorLength( trace_exit.end - data.enter_trace.end );

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;


	float lost_damage = fmaxf( 0.0f, v35 + thickness );

	if ( lost_damage > data.current_damage )
		return false;

	if ( lost_damage >= 0.0f )
		data.current_damage -= lost_damage;

	if ( data.current_damage < 1.0f )
		return false;

	data.src = trace_exit.end;
	data.penetrate_count--;
	return true;
}

bool CAutoWall::CanWallbang( float &dmg ) {

	auto local = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	auto weapon = reinterpret_cast<sdk::CBaseWeapon*>( ctx::client_ent_list->GetClientEntity( local->GetActiveWeaponIndex( ) ) );
	if ( !local )
		return false;
	twoja_stara data = twoja_stara( local->GetEyePosition( ) );
	data.filter = sdk::CTraceFilter( );
	data.filter.pSkip1 = local;
	auto flRange = weapon->get_full_info( )->flRange;
	Vector EyeAng;
	ctx::engine->GetViewAngles( EyeAng );

	Vector dst, forward;

	MATH::AngleVectors( EyeAng, &forward );
	dst = data.src + ( forward * 8000.f );

	Vector angles;
	MATH::CalcAngle( data.src, dst, angles );
	MATH::AngleVectors( angles, &data.direction );
	VectorNormalize( data.direction );



	if ( !weapon )
		return false;

	data.penetrate_count = 1;

	sdk::CSWeaponInfo *weaponData = weapon->get_full_info( );

	if ( !weaponData )
		return false;

	data.current_damage = ( int ) weaponData->iDamage;

	data.trace_length_remaining = weaponData->flRange;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	UTIL_TraceLine( data.src, end, MASK_SHOT | CONTENTS_GRATE, local, 0, &data.enter_trace );

	if ( data.enter_trace.flFraction == 1.f )
		return false;

	if ( HandleBulletPenetration( weaponData, data ) ) {
		dmg = data.current_damage;
		return true;
	}
	return false;
}

bool CAutoWall::FloatingPointIsVisible(sdk::CBaseEntity* local_player, const Vector &point)
{
	sdk::trace_t Trace;
	Vector end = point;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
	UTIL_TraceLineSig(local_position, end, MASK_SOLID, local_player, 0, &Trace);

	if (Trace.flFraction == 1.0f)
	{
		return true;
	}

	return false;
}
CAutoWall* autowall = new CAutoWall();