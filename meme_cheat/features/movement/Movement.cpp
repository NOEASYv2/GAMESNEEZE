#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "Movement.h"

void CMovement::bunnyhop(sdk::CUserCmd* cmd)
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->GetFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;

		if (local_player->GetVelocity().Length() <= 50)
			cmd->forwardmove = 450.f;
	}
}

void set_ct( const char* tag, const char* name ) {
	static auto ct_sig = UTILS::FindPattern( "engine.dll", ( PBYTE )"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx" );
	if ( ct_sig ) {
		if ( strlen( name ) > 0 ) {
			auto name_ = std::string( name );
			auto newline = name_.find( "\\n" );
			auto tab = name_.find( "\\t" );
			if ( newline != std::string::npos ) {
				name_.replace( newline, newline + 2, "\n" );
			}
			if ( tab != std::string::npos ) {
				name_.replace( tab, tab + 2, "\t" );
			}
		}
		auto tag_ = std::string( tag );
		if ( strlen( tag ) > 0 ) {
			auto newline = tag_.find( "\\n" );
			auto tab = tag_.find( "\\t" );
			if ( newline != std::string::npos ) {
				tag_.replace( newline, newline + 2, "\n" );
			}
			if ( tab != std::string::npos ) {
				tag_.replace( tab, tab + 2, "\t" );
			}
		}
		static auto set_ct = reinterpret_cast<void( __fastcall* )( const char*, const char* )>( ct_sig );
		set_ct( tag_.data( ), tag_.data( ) );
	}
}

void CMovement::set_ct( const char* tag ) {
	static auto ClanTagOffset = UTILS::FindPattern( "engine.dll", ( PBYTE )"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx" );
	if ( ClanTagOffset ) {
		auto tag_ = std::string( tag );
		if ( strlen( tag ) > 0 ) {
			auto newline = tag_.find( "\\n" );
			auto tab = tag_.find( "\\t" );
			if ( newline != std::string::npos ) {
				tag_.replace( newline, newline + 2, "\n" );
			}
			if ( tab != std::string::npos ) {
				tag_.replace( tab, tab + 2, "\t" );
			}
		}
		static auto dankesttSetClanTag = reinterpret_cast<void( __fastcall* )( const char*, const char* )>( ClanTagOffset );
		dankesttSetClanTag( tag_.data( ), tag_.data( ) );
	}
}



void CMovement::autostrafer(sdk::CUserCmd* cmd) //thanks toast
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return;

	if ( local_player->GetMoveType( ) == MOVETYPE_LADDER || local_player->GetMoveType( ) == MOVETYPE_NOCLIP )
		return;

	int local_flags = local_player->GetFlags();
	if (!(local_player->GetFlags() & local_flags & FL_ONGROUND))
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1)
			cmd->sidemove = cmd->mousedx < 0.0f ? -590 : 590;
		else 
		{
			cmd->forwardmove = 4200.0f / local_player->GetVelocity().Length();
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -590 : 590;
			if (cmd->forwardmove >  590 )
				cmd->forwardmove = 590;
		}
	}
	else if (cmd->buttons & IN_JUMP)
		cmd->forwardmove = 590;
}


CMovement* movement = new CMovement();