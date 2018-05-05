#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/NetChannel.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/ConVar.h"
#include "../autowall/AutoWall.h"
#include "../aimbot/Aimbot.h"
#include "Backtracking.h"

#define TICK_INTERVAL			( g_pGlobalVarsBase->interval_per_tick )

#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )

template<class T> const T&
clamp(const T& x, const T& upper, const T& lower) { return min(upper, max(x, lower)); }
static std::deque<CBacktrack::SequenceData>sequences;
void CBacktrack::AddLatencyToNetchan( sdk::NetChannel *netchan, float Latency ) {
	for ( auto& seq : sequences ) {
		if ( ctx::globals->realtime - seq.Realtime >= Latency ) {
			netchan->m_nInReliableState = seq.inreliablestate;
			netchan->m_nInSequenceNr = seq.sequencenr;
			break;
		}
	}
}
int lastincomingsequencenumber = 0;
void CBacktrack::UpdateIncomingSequences( ) {
	DWORD ClientState = ( DWORD )*( DWORD* ) ctx::client_state;
	if ( ClientState ) {
		sdk::NetChannel *netchan = *reinterpret_cast<sdk::NetChannel**>( reinterpret_cast<std::uintptr_t>( ctx::client_state ) + 0x9C );
		if ( netchan ) {
			if ( netchan->m_nInSequenceNr > lastincomingsequencenumber ) {
				lastincomingsequencenumber = netchan->m_nInSequenceNr;
				sequences.push_front( CBacktrack::SequenceData( netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, ctx::globals->realtime ) );
			}
			if ( sequences.size( ) > 2048 )
				sequences.pop_back( );
		}
	}
}

void CBacktrack::backtrack_player(sdk::CUserCmd* cmd)
{
	for (int i = 1; i < 65; ++i)
	{

		auto entity = ctx::client_ent_list->GetClientEntity(i);
		auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

		if (!entity)
			continue;

		if (!local_player)
			continue;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player)
			continue;

		if (is_teammate)
			continue;

		if (entity->GetHealth() <= 0)
			continue;

		if (local_player->GetHealth() <= 0)
			continue;

		if (entity->GetImmunity())
			continue;

		int index = entity->GetIndex();

		for (int i = 0; i < 20; i++)
		{
			backtrack_hitbox[index][i][cmd->command_number % 12] = aimbot->get_hitbox_pos(entity, i);
		}

		backtrack_simtime[index][cmd->command_number % 12] = entity->GetSimTime();

		for (int i = 0; i < 12; i++)
		{
			if (backtrack_simtime[index][i] != compensate[index][i])
			{
				if (i > 0 && i != 11)
				{
					oldest_tick[index] = i + 2;
				}
				else
				{
					oldest_tick[index] = 1;
				}
				compensate[index][i] = backtrack_simtime[index][i];
			}
		}
	}
}

inline Vector CBacktrack::angle_vector(Vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(M_PI));

	return Vector(cp*cy, cp*sy, -sp);
}

inline float CBacktrack::point_to_line(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

void CBacktrack::run_legit(sdk::CUserCmd* cmd) //phook backtrack muahhahahahaaha
{
	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;
	sdk::player_info_t info;

	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return;

	for (int i = 1; i < 65; i++)
	{
		auto entity = ctx::client_ent_list->GetClientEntity(i);

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		if (!ctx::engine->GetPlayerInfo(i, &info))
			continue;

		if (entity->GetIsDormant())
			continue;

		if (entity->GetTeam() == local_player->GetTeam())
			continue;

		if (!local_player->GetHealth() > 0)
			return;

		if (entity->GetHealth() > 0)
		{
			float simtime = entity->GetSimTime();
			Vector hitboxPos = aimbot->get_hitbox_pos(entity, 0);

			headPositions[i][cmd->command_number % 12] = legit_backtrackdata{ simtime, hitboxPos };
			Vector ViewDir = angle_vector(cmd->viewangles + (*local_player->GetPunchAngles() * 2.f));
			Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
			float FOVDistance = point_to_line(hitboxPos, local_position, ViewDir);

			if (bestFov > FOVDistance)
			{
				bestFov = FOVDistance;
				bestTargetIndex = i;
			}
		}
	}

	float bestTargetSimTime;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir = angle_vector(cmd->viewangles + (*local_player->GetPunchAngles() * 2.f));
		Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

		for (int t = 0; t < 12; ++t)
		{
			float tempFOVDistance = point_to_line(headPositions[bestTargetIndex][t].hitboxPos, local_position, ViewDir);
			if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > local_player->GetSimTime() - 1)
			{
				tempFloat = tempFOVDistance;
				bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
			}
		}
		if (cmd->buttons & IN_ATTACK)
		{
			cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
		}
	}
}
bool CBacktrack::IsValid(sdk::CBaseEntity * player)
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return false;

	if (!player)
		return false;

	if (player->GetIsDormant() || player->GetHealth() == 0 || player->GetFlags() & FL_FROZEN)
		return false;

	if (player->GetTeam() == local_player->GetTeam())
		return false;

	if (player->GetClientClass()->m_ClassID != 35)
		return false;

	if (player == local_player)
		return false;

	if (player->GetImmunity())
		return false;

	return true;
}

CBacktrack* backtracking = new CBacktrack();
legit_backtrackdata headPositions[64][12];