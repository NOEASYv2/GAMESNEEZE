#pragma once

#define BACKTRACKING_ENABLED

#define MAX_RECORDS 64
#define ACCELERATION_CALC_TIME 0.3f
#define ROTATION_CALC_TIME 0.5f
#define DIRECTION_CALC_TIME 0.5f
#define FLAG_CALC_TIME 0.9f

#define MAXSTUDIOBONES 128		// total bones actually used
#define MAX_PLAYERS 65

#include <deque>
#include <array>

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
	class ConVar;
}

struct legit_backtrackdata
{
	float simtime;
	Vector hitboxPos;
};

/*struct LayerRecord
{
	int m_sequence;
	float m_cycle;
	float m_weight;
	int m_order;

	LayerRecord()
	{
		m_sequence = 0;
		m_cycle = 0;
		m_weight = 0;
		m_order = 0;
	}

	LayerRecord(const LayerRecord& src)
	{
		m_sequence = src.m_sequence;
		m_cycle = src.m_cycle;
		m_weight = src.m_weight;
		m_order = src.m_order;
	}
};

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void *data;
	void *watcher;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

struct PlayerSettings
{
	bool friendly;
	float threatMultiplier; //Used in GetBestTarget to prefer the player
	float healthMultiplier; //Used in HitScan to prefer headshots or bodyaim
	float sortMultiplier; //Used to make the sort function cycle through players
	int shotCount; //Per player shot count used by resolver
	float shotCountTime; //The last time shotcount was increased, used to reset it after some time
	bool shootOnlyLBY; //Should aimbot shoot only at lby updates?
	float lbyUpdateTime; //Set by resolver, predicted lby update time
	bool useLoggedAngles; //Should use logged angles in the resolver (set automatically)
	float loggedLBYDelta; //LBY delta to keep using for a few shots
	int logFailCount; //Counts how many times the logged delta fails on second use, used to determine randomized offsets
	int shotCountLog; //Shot count when the angle was logged
	float lastSimTimeDelta; //I added this comment here
	float averageSimTimeDelta; //Used in lagcomp
	bool inairFakelag; //B1g inair only fakelags, screw em
	bool breakingLC; //Breaks lag compensation

	PlayerSettings() : friendly(false), threatMultiplier(1.0f), healthMultiplier(1.0f), sortMultiplier(1.0f), shotCount(0), shotCountTime(0), shootOnlyLBY(false), lbyUpdateTime(0), useLoggedAngles(false), loggedLBYDelta(0), logFailCount(0), shotCountLog(0), lastSimTimeDelta(.0f), averageSimTimeDelta(.0f), inairFakelag(false), breakingLC(false) {}
};

PlayerSettings g_PlayerSettings[65];
static auto cl_interp = ctx::convar->FindVar("cl_interpolate");

struct LagRecord
{
public:
	LagRecord()
	{
		m_fFlags = 0;
		m_vecOrigin.Init(0.0f, 0.0f, 0.0f);
		m_vecAngles.Init(0.0f, 0.0f, 0.0f);
		m_eyeAngles.Init(0.0f, 0.0f, 0.0f);
		m_eyePos.Init(0.0f, 0.0f, 0.0f);
		m_vecVelocity.Init(0.0f, 0.0f, 0.0f);
		m_vecMinsPreScaled.Init(0.0f, 0.0f, 0.0f);
		m_vecMaxsPreScaled.Init(0.0f, 0.0f, 0.0f);
		m_flSimulationTime = -1;
		tick_count = 0;

		for (int i = 0; i < 24; i++)
			poseparams[i] = 0;
	}

	LagRecord(sdk::CBaseEntity* player, sdk::CUserCmd* cmd)
	{
		//Lots of stuff saved that's not even restored
		m_fFlags = player->GetFlags();
		m_vecOrigin = player->GetVecOrigin();
		m_vecAngles = player->GetAbsAngles();
		m_eyePos = player->GetVecOrigin() + player->GetViewOffset();
		m_eyeAngles = player->GetEyeAngles();
		m_vecVelocity = player->GetVelocity();
		//m_vecMinsPreScaled = player->GetCollideable->OBBMins();
		//m_vecMaxsPreScaled = player->GetCollideable->OBBMaxs();
		m_flSimulationTime = player->GetSimTime();

		if (cmd)
			tick_count = cmd->tick_count;
		else
			tick_count = 0;

		std::array<float, 24>* plPoseParams = ((sdk::CBaseAnimating*)player)->m_flPoseParameter();
		for (int i = 0; i < 24; i++)
			poseparams[i] = plPoseParams->at(i);

		Vector tempOrigin = player->GetVecOrigin();
		Vector deltaOrigin = m_vecOrigin - tempOrigin;
		Vector tempAngles = player->GetAbsAngles();
		Vector deltaAngles = tempAngles - m_vecAngles;

		//InvalidateBoneCache(player);

		player->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, ctx::globals->curtime);

		if (false && cl_interp && cl_interp->GetInt())
		{
			//When not disabling interp, this moves the bonematrix to the vecOrigin and rotates as well.
			for (int i = 0; i < 128; i++) {
				boneMatrix[i][0][3] += deltaOrigin.x;
				boneMatrix[i][1][3] += deltaOrigin.y;
				boneMatrix[i][2][3] += deltaOrigin.z;

				//Rotate the transofrm matrix
				Vector newPos = MATH::RotateVectorYaw(Vector(0, 0, 0), deltaAngles.y, Vector(boneMatrix[i][0][0], boneMatrix[i][1][0], boneMatrix[i][2][0]));
				boneMatrix[i][0][0] = newPos.x;
				boneMatrix[i][1][0] = newPos.y;
				boneMatrix[i][2][0] = newPos.z;

				newPos = MATH::RotateVectorYaw(Vector(0, 0, 0), deltaAngles.y, Vector(boneMatrix[i][0][1], boneMatrix[i][1][1], boneMatrix[i][2][1]));
				boneMatrix[i][0][1] = newPos.x;
				boneMatrix[i][1][1] = newPos.y;
				boneMatrix[i][2][1] = newPos.z;

				newPos = MATH::RotateVectorYaw(Vector(0, 0, 0), deltaAngles.y, Vector(boneMatrix[i][0][2], boneMatrix[i][1][2], boneMatrix[i][2][2]));
				boneMatrix[i][0][2] = newPos.x;
				boneMatrix[i][1][2] = newPos.y;
				boneMatrix[i][2][2] = newPos.z;

				newPos = MATH::RotateVectorYaw(m_vecOrigin, deltaAngles.y, Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]));
				boneMatrix[i][0][3] = newPos.x;
				boneMatrix[i][1][3] = newPos.y;
				boneMatrix[i][2][3] = newPos.z;
			}
		}

	}

	LagRecord(const LagRecord& src, Vector origin, float addSimTime) : LagRecord(src)
	{
		Vector deltaOrigin = origin - m_vecOrigin;

		m_vecOrigin = origin;

		m_flSimulationTime += addSimTime;

		//When not disabling interp, this moves the bonematrix to the vecOrigin and rotates as well.
		for (int i = 0; i < 128; i++) {
			boneMatrix[i][0][3] += deltaOrigin.x;
			boneMatrix[i][1][3] += deltaOrigin.y;
			boneMatrix[i][2][3] += deltaOrigin.z;
		}
	}

	LagRecord(const LagRecord& src)
	{
		m_fFlags = src.m_fFlags;
		m_vecOrigin = src.m_vecOrigin;
		m_vecAngles = src.m_vecAngles;
		m_eyePos = src.m_eyePos;
		m_eyeAngles = src.m_eyeAngles;
		m_vecVelocity = src.m_vecVelocity;
		m_vecMinsPreScaled = src.m_vecMinsPreScaled;
		m_vecMaxsPreScaled = src.m_vecMaxsPreScaled;
		m_flSimulationTime = src.m_flSimulationTime;
		tick_count = src.tick_count;
		lbyUpdate = src.lbyUpdate;

		for (int i = 0; i < 24; i++)
			poseparams[i] = src.poseparams[i];

		memcpy(boneMatrix, src.boneMatrix, sizeof(matrix3x4_t[MAXSTUDIOBONES]));
	}

	int m_fFlags;

	Vector m_vecOrigin;
	Vector m_vecAngles;
	Vector m_eyePos;
	Vector m_eyeAngles;
	Vector m_vecVelocity;
	Vector m_vecMinsPreScaled;
	Vector m_vecMaxsPreScaled;

	float m_flSimulationTime;
	int tick_count;

	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	float poseparams[24];

	bool lbyUpdate;
	bool isBunnyhopping;
	float lbyDelta;
};

namespace LagCompensation
{
	extern std::deque<LagRecord> m_PlayerTrack[MAX_PLAYERS];
	extern std::deque<LagRecord> m_PlayerTrack_Future[MAX_PLAYERS];

	extern LagRecord m_RestoreData;
	extern sdk::CBaseEntity* m_pCurrentPlayer;

	extern void CreateMove(sdk::CUserCmd* cmd);
	extern void StartLagCompensation(sdk::CBaseEntity* player);
	extern void BacktrackPlayer(sdk::CBaseEntity* player, LagRecord record, bool setAngles = true);
	extern void EndLagCompensation(sdk::CBaseEntity* player);
}
*/
class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void *data;
	void *watcher;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

class CBacktrack
{
public:
	struct SequenceData {
		SequenceData::SequenceData( int instate, int outstate, int seqnr, float realtime ) {
			inreliablestate = instate;
			outreliablestate = outstate;
			sequencenr = seqnr;
			Realtime = realtime;
		}
		int inreliablestate;
		int outreliablestate;
		int sequencenr;
		float Realtime;
	};
	void AddLatencyToNetchan( sdk::NetChannel * netchan, float Latency );
	void UpdateIncomingSequences( );
	void backtrack_player(sdk::CUserCmd * cmd);
	void run_legit(sdk::CUserCmd * cmd);
	void DisableInterpolation(sdk::CBaseEntity* pEntity)
	{
		VarMapping_t* map = GetVarMap(pEntity);
		if (!map) return;
		for (int i = 0; i < map->m_nInterpolatedEntries; i++)
		{
			VarMapEntry_t *e = &map->m_Entries[i];
			e->m_bNeedsToInterpolate = false;
		}
	}

private:
	VarMapping_t * GetVarMap(void* pBaseEntity)
	{
		return reinterpret_cast<VarMapping_t*>((DWORD)pBaseEntity + 0x24); 
	}
	int latest_tick;
	Vector angle_vector(Vector meme);
	float point_to_line(Vector Point, Vector LineOrigin, Vector Dir);
	bool IsValid(sdk::CBaseEntity * player);
};

extern legit_backtrackdata headPositions[64][12];
extern CBacktrack* backtracking;
