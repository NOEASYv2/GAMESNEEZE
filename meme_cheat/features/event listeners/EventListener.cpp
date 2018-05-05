#include "../../includes.h"

#include "../../utilities/interfaces.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/ISurface.h"
#include "../../utilities/render.h"
#include <deque>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

#include "../../features/lag compensation/Backtracking.h"
#include "../../features/resolver/Resolver.h"
#include "sound.h"
#include "../../features/visual/Visuals.h"

#include "EventListener.h"
#include <playsoundapi.h>

#pragma comment(lib, "Winmm.lib")

bool did_hit = false;

CGameEvents::PlayerHurtListener player_hurt_listener;
CGameEvents::PlayerHurtListener player_death_listener;
CGameEvents::BulletImpactListener bullet_impact_listener;
CGameEvents::ItemPurchaseListener item_purchase_listener;

void CGameEvents::InitializeEventListeners( ) {
	ctx::game_event_manager->AddListener( &player_hurt_listener, "player_hurt", false );
	ctx::game_event_manager->AddListener( &player_death_listener, "player_death", false );
	ctx::game_event_manager->AddListener( &bullet_impact_listener, "bullet_impact", false );
	ctx::game_event_manager->AddListener( &item_purchase_listener, "item_purchase", false );
}

void CGameEvents::RemoveListeners( ) {
	ctx::game_event_manager->RemoveListener( &player_hurt_listener );
	ctx::game_event_manager->RemoveListener( &player_death_listener );
	ctx::game_event_manager->RemoveListener( &bullet_impact_listener );
	ctx::game_event_manager->RemoveListener( &item_purchase_listener );
}
template <typename t> t xd( t value, t min, t max ) {
	if ( value > max ) {
		return max;
	}
	if ( value < min ) {
		return min;
	}
	return value;
}
void CGameEvents::PlayerHurtListener::FireGameEvent( sdk::IGameEvent* game_event ) {
	if ( !game_event )
		return;

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	if ( !local_player )
		return;

	auto entity = ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "userid" ) ) );
	if ( !entity )
		return;

	auto entity_attacker = ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "attacker" ) ) );
	if ( !entity_attacker )
		return;

	if ( entity->GetTeam( ) == local_player->GetTeam( ) )
		return;

	if ( !local_player->GetHealth( ) > 0 )
		return;

	sdk::player_info_t player_info;
	if ( !ctx::engine->GetPlayerInfo( entity->GetIndex( ), &player_info ) )
		return;

	sdk::CBaseEntity* hurt = ( sdk::CBaseEntity* ) ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "userid" ) ) );

	if ( entity_attacker == local_player ) {
		hitmarker_time = 255;

		switch ( Vars::options.hitsound ) {
			case 0:
				break;
			case 1:
				ctx::surface->IPlaySound( "buttons\\arena_switch_press_02.wav" );
				break;
			case 2:
				PlaySoundA( ( LPCSTR ) rawData, NULL, SND_MEMORY | SND_ASYNC );
				break;
			case 3:
				int hitgroup = game_event->GetInt( "hitgroup" );
				if ( hitgroup == 1 )
					ctx::surface->IPlaySound( "buttons\\arena_switch_press_02.wav" );
				else if ( hitgroup == 2 || 3 || 4 || 5 || 6 || 7 )
					PlaySoundA( ( LPCSTR ) rawData, NULL, SND_MEMORY | SND_ASYNC );
				break;
		}
	}

	if ( strcmp( game_event->GetName( ), "player_hurt" ) == 0 ) {

		if ( hurt != local_player && entity_attacker == local_player ) {
			CVisuals::damage_indicator_t DmgIndicator;
			DmgIndicator.dmg = game_event->GetInt( "dmg_health" );
			DmgIndicator.player = hurt;
			DmgIndicator.earse_time = local_player->GetTickBase( ) * ctx::globals->interval_per_tick + 3.f;
			DmgIndicator.initializes = false;

			visuals->dmg_indicator.push_back( DmgIndicator );
		}
	}
	const char* team [ ] { "",""," (T)"," (CT)" };

	if ( strcmp( game_event->GetName( ), "player_hurt" ) == 0 ) {
		auto userid = game_event->GetInt( "userid" );
		auto attacker = game_event->GetInt( "attacker" );

		if ( !userid || !attacker )
			return;

		auto userid_id = ctx::engine->GetPlayerForUserID( userid );
		auto attacker_id = ctx::engine->GetPlayerForUserID( attacker );

		sdk::player_info_t userid_info, attacker_info;
		if ( !ctx::engine->GetPlayerInfo( userid_id, &userid_info ) )
			return;

		if ( !ctx::engine->GetPlayerInfo( attacker_id, &attacker_info ) )
			return;

		int damage = game_event->GetInt( "dmg_health" );
		std::stringstream string;

		string << attacker_info.name << " hit " << userid_info.name << " for " << damage << " (" << std::to_string( xd( hurt->GetHealth( ) - damage, 0, 100 ) ) << " health remaining).";
		visuals->events.push_front( CVisuals::loginfo( string.str( ), CColor( 255, 255, 255, 255 ), ctx::globals->curtime ) );

		const char* msg = string.str( ).c_str( );

		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "[" ) );
		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "gamesneeze" ) );
		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "] " ) );
		ctx::convar->ConsoleColorPrintf( CColor( 255, 255, 255, 255 ), ( string.str( ).c_str( ) ) );
		ctx::convar->ConsoleColorPrintf( CColor( 255, 255, 255, 255 ), ( "\n" ) );

		if ( visuals->events.size( ) > 12 )
			visuals->events.pop_back( );
	}

}

void CGameEvents::PlayerDeathListener::FireGameEvent( sdk::IGameEvent* game_event ) {
	if ( !game_event )
		return;

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	if ( !local_player )
		return;

	auto entity_attacker = ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "attacker" ) ) );

	if ( !local_player->GetHealth( ) > 0 )
		return;


}

void CGameEvents::BulletImpactListener::FireGameEvent( sdk::IGameEvent* game_event ) {
	if ( !game_event )
		return;

	int iUser = ctx::engine->GetPlayerForUserID( game_event->GetInt( "userid" ) );

	auto entity = reinterpret_cast< sdk::CBaseEntity* >( ctx::client_ent_list->GetClientEntity( iUser ) );

	if ( !entity )
		return;

	if ( entity->GetIsDormant( ) )
		return;
}

void CGameEvents::ItemPurchaseListener::FireGameEvent( sdk::IGameEvent* game_event ) {
	if ( !game_event )
		return;

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	if ( !local_player )
		return;

	auto entity_attacker = ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "attacker" ) ) );

	auto entity = ctx::client_ent_list->GetClientEntity( ctx::engine->GetPlayerForUserID( game_event->GetInt( "userid" ) ) );
	if ( !entity )
		return;

	sdk::player_info_t player_info;
	if ( !ctx::engine->GetPlayerInfo( entity->GetIndex( ), &player_info ) )
		return;

	if ( entity->GetTeam( ) == local_player->GetTeam( ) )
		return;

	if ( !local_player->GetHealth( ) > 0 )
		return;

/*	const char* team [ ] { "",""," [T]"," [CT]" };
	if ( strcmp( game_event->GetName( ), "item_purchase" ) == 0 ) {
		auto buyer = game_event->GetInt( "userid" );
		auto player_index = ctx::engine->GetPlayerForUserID( buyer );
		sdk::CBaseEntity* player = ( sdk::CBaseEntity* ) ctx::client_ent_list->GetClientEntity( player_index );
		std::stringstream string;

		std::string gun = game_event->GetString( "weapon" );

		string << player_info.name << " bought a " << gun.c_str( );
		visuals->events.push_front( CVisuals::loginfo( string.str( ), CColor( 255, 255, 255, 255 ), ctx::globals->curtime ) );

		const char* msg = string.str( ).c_str( );

		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "[" ) );
		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "gamesneeze" ) );
		ctx::convar->ConsoleColorPrintf( Vars::options.menu_col, ( "] " ) );
		ctx::convar->ConsoleColorPrintf( CColor( 255, 255, 255, 255 ), ( string.str( ).c_str( ) ) );
		ctx::convar->ConsoleColorPrintf( CColor( 255, 255, 255, 255 ), ( "\n" ) );

		if ( visuals->events.size( ) > 12 )
			visuals->events.pop_back( );
	}*/
}
