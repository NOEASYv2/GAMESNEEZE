#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseAnimState.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../features/aimbot/Aimbot.h"
#include "AntiAim.h"

float randnum( float Min, float Max ) {
	return ( ( float( rand( ) ) / float( RAND_MAX ) ) * ( Max - Min ) ) + Min;
}

float get_curtime( sdk::CUserCmd* ucmd ) {
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

float anim_velocity( sdk::CBaseEntity* LocalPlayer ) {
	if ( LocalPlayer->GetAnimState( ) == nullptr )
		return false;

	int vel = LocalPlayer->GetAnimState( )->speed_2d;
	return vel;
}

bool first_supress( const float yaw_to_break, sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime( cmd );

	auto animstate = local_player->GetAnimState( );
	if ( !animstate )
		return false;

	if ( !( local_player->GetFlags( ) & FL_ONGROUND ) )
		return false;

	if ( Vars::options.delta_val < 120 )
		return false;

	if ( animstate->speed_2d > 0.1 )
		next_lby_update_time = curtime + 0.22 - TICKS_TO_TIME( 1 );

	if ( next_lby_update_time < curtime ) {
		next_lby_update_time = curtime + 1.1;
		return true;
	}

	return false;
}

bool second_supress( const float yaw_to_break, sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime( cmd );

	auto animstate = local_player->GetAnimState( );
	if ( !animstate )
		return false;

	if ( !( local_player->GetFlags( ) & FL_ONGROUND ) )
		return false;

	if ( Vars::options.delta_val < 120 )
		return false;

	if ( animstate->speed_2d > 0.1 )
		next_lby_update_time = curtime + 0.22 + TICKS_TO_TIME( 1 );


	if ( next_lby_update_time < curtime ) {
		next_lby_update_time = curtime + 1.1;
		return true;
	}

	return false;
}

bool next_lby_update( const float yaw_to_break, sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return false;

	static float next_lby_update_time = 0;
	float curtime = get_curtime( cmd );

	auto animstate = local_player->GetAnimState( );
	if ( !animstate )
		return false;

	if ( !( local_player->GetFlags( ) & FL_ONGROUND ) )
		return false;

	if ( animstate->speed_2d > 0.1 )
		next_lby_update_time = curtime + 0.22f;

	if ( next_lby_update_time < curtime ) {
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

float Distance( Vector a, Vector b ) {
	return sqrt( pow( a.x - b.x, 2 ) + pow( a.y - b.y, 2 ) + pow( a.z - b.z, 2 ) );
}

bool CAntiAim::GetBestHeadAngle( Vector& angle ) {
	auto localplayer = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	Vector position = localplayer->GetVecOrigin( ) + localplayer->GetViewOffset( );

	float closest_distance = 100.0f;

	float radius = Vars::options.edge_distance + 0.1f;
	float step = M_PI * 2.0 / 8;

	for ( float a = 0; a < ( M_PI * 2.0 ); a += step ) {
		Vector location( radius * cos( a ) + position.x, radius * sin( a ) + position.y, position.z );

		sdk::Ray_t ray;
		sdk::trace_t tr;
		ray.Init( position, location );
		sdk::CTraceFilter traceFilter;
		traceFilter.pSkip1 = localplayer;
		ctx::trace->TraceRay( ray, 0x4600400B, &traceFilter, &tr );
		
		float distance = Distance( position, tr.end );

		if ( distance < closest_distance ) {
			closest_distance = distance;
			angle.y = RAD2DEG( a );
		}
	}

	return closest_distance < Vars::options.edge_distance;
}

void CAntiAim::antiaimyaw( sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	switch ( Vars::options.aa_type % 8 ) {
		case 0:
			//skeet unhittable aa no leak please
			break;
		case 1:
			backwards( cmd ); //sideways
			break;
		case 2:
			sideways( cmd ); //backwards
			break;
		case 3:
			backjitter( cmd ); //legitaa
			break;
		case 4:
			legit( cmd ); //lby breaker
			break;
		case 5: //slowspin
			if ( !GLOBAL::should_send_packet ) {
				float server_time = local_player->GetTickBase( ) * ctx::globals->interval_per_tick;
				double rate = 360.0 / 1.618033988749895;
				double yaw = fmod( static_cast<double>( server_time )*rate, 360.0 );
				cmd->viewangles.y += static_cast<float>( yaw );
			}//pasted from phook (best cheat for code!!!!!!!)
			break;

	}
}
bool isknife( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	auto weapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) ) );

	if ( weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_BAYONET ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_BOWIE ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_CT ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_FALCHION ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_FLIP ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_GUT ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_PUSH ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_T ||
		weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL )
		return true;
	else
		return false;
}
void CAntiAim::do_antiaim( sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	auto weapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) ) );

	if ( !weapon )
		return;

	if ( cmd->buttons & IN_USE )
		return;

	if ( local_player->GetMoveType( ) == MOVETYPE_LADDER || local_player->GetMoveType( ) == MOVETYPE_NOCLIP )
		return;

	if ( cmd->buttons & IN_ATTACK && aimbot->can_shoot( ) )
		return;

	if ( isknife( )  )
		return;

	if ( weapon->get_full_info( )->WeaponType == 9 )
		return;
		

	Vector angle = cmd->viewangles;

	Vector edge_angle = angle;
	bool edging_head = Vars::options.aa_autodir && GetBestHeadAngle( edge_angle );

	if ( Vars::options.aa_bool ) {
		antiaimyaw( cmd );
		MATH::NormalizeAngle(angle);
	}

	if (Vars::options.lby_breaker_bool)
	{
		static float last_real;
		float lbydelta = Vars::options.delta_val;

		if (!GLOBAL::should_send_packet)
		{
			if ( next_lby_update( cmd->viewangles.y - lbydelta, cmd ) ) {
					cmd->viewangles.y = last_real - lbydelta;
			}		

			last_real = cmd->viewangles.y;
		}
	}		

	switch ( Vars::options.aa_type_x % 6 ) {
		case 0:
			//skeet unhittable aa no leak please
			break;
		case 1:
			cmd->viewangles.x = 89.000000; //down
			break;
		case 2:
			cmd->viewangles.x = -89.000000; //up
			break;
		case 3:
			cmd->viewangles.x = 1080.000000; //fake zero (down)
			break;
		case 4:
			cmd->viewangles.x = -1080.000000; //fake zero (up)
			break;
		case 5:
			cmd->viewangles.x = -3432243424.000000; //very secret not public pitch that you cannot 10000001% hit!!!!!
			break;
	}

	switch ( Vars::options.aa_type_fake % 6 ) {
		case 0:
			//skeet unhittable aa no leak please
			break;
		case 1:
			if ( GLOBAL::should_send_packet ) {
				if ( Vars::options.flip_side == 0 ) {
					cmd->viewangles.y += 180;
				}
				if ( Vars::options.flip_side == 1 ) {
					cmd->viewangles.y -= 180;
				}
			}
			break;
		case 2: //sideways
			if ( GLOBAL::should_send_packet ) {
				if ( Vars::options.flip_side == 0 ) {
					cmd->viewangles.y += 90;
				}
				if ( Vars::options.flip_side == 1 ) {
					cmd->viewangles.y -= 90;
				}
			}
			break;
		case 3: //backjitter
			if ( GLOBAL::should_send_packet ) {
				if ( Vars::options.flip_side == 0 ) {
					cmd->viewangles.y += 180 + ( ( rand( ) % 15 ) - ( 15 * 0.5f ) );
				}
				if ( Vars::options.flip_side == 1 ) {
					cmd->viewangles.y -= 180 + ( ( rand( ) % 15 ) - ( 15 * 0.5f ) );
				}
			}
			break;
		case 4: //slowspin 
			if ( GLOBAL::should_send_packet ) {
				float server_time = local_player->GetTickBase( ) * ctx::globals->interval_per_tick;
				double rate = 360.0 / 1.618033988749895;
				double yaw = fmod( static_cast<double>( server_time )*rate, 360.0 );
				cmd->viewangles.y += static_cast<float>( yaw );
			} //pasted from phook (best cheat for code!!!!!!!)
			break;
		case 5: //sidejitter
			if ( GLOBAL::should_send_packet ) {
				if ( Vars::options.flip_side == 0 ) {
					cmd->viewangles.y += 90 + ( ( rand( ) % 15 ) - ( 15 * 0.5f ) );
				}
				if ( Vars::options.flip_side == 1 ) {
					cmd->viewangles.y -= 90 + ( ( rand( ) % 15 ) - ( 15 * 0.5f ) );
				}
			}
			break;
	}
}

void CAntiAim::fix_movement( sdk::CUserCmd* cmd ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	Vector real_viewangles;
	ctx::engine->GetViewAngles( real_viewangles );

	Vector vecMove( cmd->forwardmove, cmd->sidemove, cmd->upmove );
	float speed = sqrt( vecMove.x * vecMove.x + vecMove.y * vecMove.y );

	Vector angMove;
	MATH::VectorAngles( vecMove, angMove );

	float yaw = DEG2RAD( cmd->viewangles.y - real_viewangles.y + angMove.y );

	cmd->forwardmove = cos( yaw ) * speed;
	cmd->sidemove = sin( yaw ) * speed;


	cmd->viewangles = MATH::NormalizeAngle( cmd->viewangles );
}

void CAntiAim::backwards( sdk::CUserCmd* cmd ) {
	if ( !GLOBAL::should_send_packet )
		cmd->viewangles.y += 180.000000;
}

void CAntiAim::legit( sdk::CUserCmd* cmd ) {
	if ( !GLOBAL::should_send_packet )
		cmd->viewangles.y += 90;
}

void CAntiAim::sideways( sdk::CUserCmd* cmd )
{
	if ( Vars::options.flip_side == 0 ) {
		if ( !GLOBAL::should_send_packet )
			cmd->viewangles.y -= 90;
	}
			
	if ( Vars::options.flip_side == 1 ) {
		if ( !GLOBAL::should_send_packet )
			cmd->viewangles.y += 90;
	}
}

void CAntiAim::backjitter( sdk::CUserCmd* cmd ) {
	if ( !GLOBAL::should_send_packet )
		cmd->viewangles.y += 180 + ( ( rand( ) % 15 ) - ( 15 * 0.5f ) );
}

CAntiAim* antiaim = new CAntiAim( );