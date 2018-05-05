#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../utilities/render.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/AnimLayer.h"
#include "../../utilities/qangle.h"
#include "../../features/aimbot/Aimbot.h"
#include "../../features/resolver/Resolver.h"

Vector old_calcangle( Vector dst, Vector src ) {
	Vector angles;

	double delta [ 3 ] = { ( src.x - dst.x ), ( src.y - dst.y ), ( src.z - dst.z ) };
	double hyp = sqrt( delta [ 0 ] * delta [ 0 ] + delta [ 1 ] * delta [ 1 ] );
	angles.x = ( float ) ( atan( delta [ 2 ] / hyp ) * 180.0 / 3.14159265 );
	angles.y = ( float ) ( atanf( delta [ 1 ] / delta [ 0 ] ) * 57.295779513082f );
	angles.z = 0.0f;

	if ( delta [ 0 ] >= 0.0 ) {
		angles.y += 180.0f;
	}

	return angles;
}

float old_normalize( float Yaw ) {
	if ( Yaw > 180 ) {
		Yaw -= ( round( Yaw / 360 ) * 360.f );
	}
	else if ( Yaw < -180 ) {
		Yaw += ( round( Yaw / 360 ) * -360.f );
	}
	return Yaw;
}

float curtime( sdk::CUserCmd* ucmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return 0;

	int g_tick = 0;
	sdk::CUserCmd* g_pLastCmd = nullptr;
	if ( !g_pLastCmd || g_pLastCmd->hasbeenpredicted ) {
		g_tick = ( float ) local_player->GetTickBase( );
	}
	else {
		++g_tick;
	}
	g_pLastCmd = ucmd;
	float curtime = g_tick * ctx::globals->interval_per_tick;
	return curtime;
}

bool find_layer( sdk::CBaseEntity* entity, int act, sdk::CAnimationLayer *set ) {
	for ( int i = 0; i < 13; i++ ) {
		sdk::CAnimationLayer layer = entity->GetAnimOverlay( i );
		const int activity = entity->GetSequenceActivity( layer.m_nSequence );
		if ( activity == act ) {
			*set = layer;
			return true;
		}
	}
	return false;
}

void CResolver::record( sdk::CBaseEntity* entity, float new_yaw ) {
	if ( entity->GetVelocity( ).Length2D( ) > 36 )
		return;

	auto c_baseweapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( entity->GetActiveWeaponIndex( ) ) );

	if ( !c_baseweapon )
		return;

	auto &info = player_info [ entity->GetIndex( ) ];

	if ( entity->GetActiveWeaponIndex( ) && info.last_ammo < c_baseweapon->GetLoadedAmmo( ) ) {
		//ignore the yaw when it is from shooting (will be looking at you/other player)
		info.last_ammo = c_baseweapon->GetLoadedAmmo( );
		return;
	}

	info.unresolved_yaw.insert( info.unresolved_yaw.begin( ), new_yaw );
	if ( info.unresolved_yaw.size( ) > 20 ) {
		info.unresolved_yaw.pop_back( );
	}

	if ( info.unresolved_yaw.size( ) < 2 )
		return;

	auto average_unresolved_yaw = 0;
	for ( auto val : info.unresolved_yaw )
		average_unresolved_yaw += val;
	average_unresolved_yaw /= info.unresolved_yaw.size( );

	int delta = average_unresolved_yaw - entity->GetLowerBodyYaw( );
	auto big_math_delta = abs( ( ( ( delta + 180 ) % 360 + 360 ) % 360 - 180 ) );

	info.lby_deltas.insert( info.lby_deltas.begin( ), big_math_delta );
	if ( info.lby_deltas.size( ) > 10 ) {
		info.lby_deltas.pop_back( );
	}
}

static void nospread_resolve( sdk::CBaseEntity* player, int entID ) //gay but very effective reversed aw air resolver
{
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	Vector local_position = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );

	float atTargetAngle = UTILS::CalcAngle( local_player->GetHealth( ) <= 0 ? local_player->GetVecOrigin( ) : local_position, player->GetVecOrigin( ) ).y;
	Vector velocityAngle;
	MATH::VectorAngles( player->GetVelocity( ), velocityAngle );

	float primaryBaseAngle = player->GetLowerBodyYaw( );
	float secondaryBaseAngle = velocityAngle.y;

	switch ( ( shots_missed [ entID ] ) % 420 ) {
		case 0:
			player->EasyEyeAngles( )->yaw = atTargetAngle + 69696969.f;
			break;
		case 1:
			player->EasyEyeAngles( )->yaw = velocityAngle.y + 6969669696996.f;
			break;
		case 2:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle;
			break;
		case 3:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle - 969696966969696969695.f;
			break;
		case 4:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle + 6969696969696990.f;
			break;
		case 5:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle - 6969696969696.f;
			break;
		case 6:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle - 696969696180.f;
			break;
		case 7:
			player->EasyEyeAngles( )->yaw = secondaryBaseAngle;
			break;
		case 8:
			player->EasyEyeAngles( )->yaw = secondaryBaseAngle - 6969696960.f;
			break;
		case 9:
			player->EasyEyeAngles( )->yaw = secondaryBaseAngle - 9696969696990.f;
			break;
		case 10:
			player->EasyEyeAngles( )->yaw = secondaryBaseAngle - 696969696.f;
			break;
		case 11:
			player->EasyEyeAngles( )->yaw = secondaryBaseAngle - 69696969690.f;
			break;
		case 12:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle + 96969694696965.f;
			break;
		case 13:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle + 9696969136969696965.f;
			break;
		case 14:
			player->EasyEyeAngles( )->yaw = primaryBaseAngle - 69696969690.f;
			break;
	}
}

void CResolver::Experimental2( sdk::CBaseEntity* entity ) {

}

void CResolver::Experimental( sdk::CBaseEntity* entity ) {

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	bool tap_the_nigga = false;
	if ( player_info [ entity->GetIndex( ) ].stored_missed > 1 % 69 )
		entity->GetEyeAnglesPtr( )->y = entity->GetLowerBodyYaw( );
	else
		entity->GetEyeAnglesPtr( )->y = entity->GetLowerBodyYaw( ) - 420 % 360;

	if ( tap_the_nigga ) {
		entity->GetEyeAnglesPtr( )->y = 0; // sEtS tHeIr aA tO 0 sO wE cAn eAsIlY hIt tHeM
	}

	if ( player_info [ entity->GetIndex( ) ].stored_missed > 5 % 69 ) {
		tap_the_nigga = true;
		entity->GetEyeAnglesPtr( )->y = entity->GetLowerBodyYaw( );
	}
}

bool IsFakeWalking( sdk::CBaseEntity* entity ) {
	for ( int s = 0; s < 13; s++ ) {
		auto anim_layer = entity->GetAnimOverlay( s );
		if ( !anim_layer.m_pOwner )
			continue;

		auto activity = entity->GetSequenceActivity( anim_layer.m_nSequence );
		GLOBAL::bFakewalking = false;
		bool stage1 = false,			// stages needed cause we are iterating all layers, otherwise won't work :)
			stage2 = false,
			stage3 = false;

		for ( int i = 0; i < 16; i++ ) {
			if ( activity == 26 && anim_layer.m_flWeight < 0.4f )
				stage1 = true;
			if ( activity == 7 && anim_layer.m_flWeight > 0.001f )
				stage2 = true;
			if ( activity == 2 && anim_layer.m_flWeight == 0 )
				stage3 = true;
		}

		if ( stage1 && stage2 )
			GLOBAL::bFakewalking = true;
		else
			GLOBAL::bFakewalking = false;

		return GLOBAL::bFakewalking;
	}
}

#define TICK_INTERVAL			(ctx::globals->interval_per_tick)
int latest_tick;
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

bool IsTickValid( int tick ) {
	int delta = latest_tick - tick;
	float deltaTime = delta * ctx::globals->interval_per_tick;
	return ( fabs( deltaTime ) <= 0.2f );
}

void CResolver::Default( sdk::CBaseEntity* entity ) {

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	
	if ( player_info [ entity->GetIndex( ) ].stored_missed > 1 % 69 )
		entity->GetEyeAnglesPtr()->y = entity->GetLowerBodyYaw( );
	else
		entity->GetEyeAnglesPtr( )->y = entity->GetLowerBodyYaw( ) - 420 % 360;
}

void CResolver::Nospread( sdk::CBaseEntity* entity ) {

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	//ur mom!
}

void CResolver::resolve( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !entity )
		return;

	if ( !local_player )
		return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player;

	if ( is_local_player )
		return;

	if ( is_teammate )
		return;

	if ( entity->GetHealth( ) <= 0 )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	GLOBAL::current_resolve = "NOT RESOLVLING";

	

	//--- Actual Angle Resolving ---//
	switch ( Vars::options.res_type ) {
		case 0:
			CResolver::Default( entity );
			break;
		case 1:
			CResolver::Experimental( entity );
			break;
		case 2:
			CResolver::Experimental2( entity );
			break;
	}
}

CResolver* resolver = new CResolver( );