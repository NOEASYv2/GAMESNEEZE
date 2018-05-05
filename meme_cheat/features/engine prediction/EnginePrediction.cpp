
#include "EnginePrediction.h"
float m_flOldCurtime;
float m_flOldFrametime;
sdk::CMoveData m_MoveData;

int* m_pPredictionRandomSeed;

void  CPredictionSystem::StartPrediction( sdk::CUserCmd* pCmd ) {
	auto pLocal = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	static bool bInit = false;
	if ( !bInit ) {
		m_pPredictionRandomSeed = *( int** ) ( UTILS::FindPattern( "client.dll", ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\xBA\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04", "xx????x????x????xxx" ) + 2 );
		bInit = true;
	}
	*m_pPredictionRandomSeed = pCmd->random_seed;
	m_flOldCurtime = ctx::globals->curtime;
	m_flOldFrametime = ctx::globals->frametime;

	ctx::globals->curtime = pLocal->GetTickBase( ) * ctx::globals->interval_per_tick;
	ctx::globals->frametime = ctx::globals->interval_per_tick;

	ctx::game_movement->StartTrackPredictionErrors( pLocal );

	memset( &m_MoveData, 0, sizeof( m_MoveData ) );
	ctx::move_helper->SetHost( pLocal );
	ctx::prediction->SetupMove( pLocal, pCmd, ctx::move_helper, &m_MoveData );
	ctx::game_movement->ProcessMovement( pLocal, &m_MoveData );
	ctx::prediction->FinishMove( pLocal, pCmd, &m_MoveData );

}

void CPredictionSystem::EndPrediction( sdk::CUserCmd* pCmd ) {
	auto pLocal = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	ctx::game_movement->FinishTrackPredictionErrors( pLocal );
	ctx::move_helper->SetHost( 0 );

	*m_pPredictionRandomSeed = -1;

	ctx::globals->curtime = m_flOldCurtime;
	ctx::globals->frametime = m_flOldFrametime;
}

CPredictionSystem* prediction_system = new CPredictionSystem( );