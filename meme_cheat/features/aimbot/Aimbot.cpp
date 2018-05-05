#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/ConVar.h"
#include "../autowall/AutoWall.h"
#include "../lag compensation/Backtracking.h"
#include "Aimbot.h"

//--- Variable Initaliztion ---//
int bestHitbox = -1, mostDamage;
Vector multipoints[128];
int multipointCount = 0;
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

bool CAimbot::spread_limit(sdk::CBaseWeapon* weapon)
{
	if (!weapon) return false;
	float inaccuracy = weapon->GetInaccuracy();
	return inaccuracy < (float)15 / 1000.f;
}

void CAimbot::rotate_movement(float yaw, sdk::CUserCmd* cmd)
{
	Vector viewangles;
	ctx::engine->GetViewAngles(viewangles);

	float rotation = DEG2RAD(viewangles.y - yaw);

	float cos_rot = cos(rotation);
	float sin_rot = sin(rotation);

	float new_forwardmove = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
	float new_sidemove = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

	cmd->forwardmove = new_forwardmove;
	cmd->sidemove = new_sidemove;
}

float CAimbot::seedchance(Vector Point)
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return 0;

	auto weapon = reinterpret_cast<sdk::CBaseWeapon*>(ctx::client_ent_list->GetClientEntity(local_player->GetActiveWeaponIndex()));

	if (!weapon)
		return 0;

	float SpreadCone = weapon->GetInaccuracy() * 256.0f / M_PI + weapon->get_full_info()->flInaccuracyMove * local_player->GetVelocity().Length() / 3000.0f;
	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

	float a = (Point - local_position).Length();
	float b = sqrt(tan(SpreadCone * M_PI / 180.0f) * a);
	if (2.2f > b) return 100.0f;
	return (2.2f / fmax(b, 2.2f)) * 100.0f;
}

int lerped_ticks()
{
	static const auto cl_interp_ratio = ctx::convar->FindVar("cl_interp_ratio");
	static const auto cl_updaterate = ctx::convar->FindVar("cl_updaterate");
	static const auto cl_interp = ctx::convar->FindVar("cl_interp");

	return TIME_TO_TICKS(max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat()));
}

static sdk::ConVar *big_ud_rate = nullptr;
static sdk::ConVar *min_ud_rate = nullptr;
static sdk::ConVar *max_ud_rate = nullptr;
static sdk::ConVar *interp_ratio = nullptr;
static sdk::ConVar *cl_interp = nullptr;
static sdk::ConVar *cl_min_interp = nullptr;
static sdk::ConVar *cl_max_interp = nullptr;

float LerpTime()
{
	static sdk::ConVar* updaterate = ctx::convar->FindVar("cl_updaterate");
	static sdk::ConVar* minupdate = ctx::convar->FindVar("sv_minupdaterate");
	static sdk::ConVar* maxupdate = ctx::convar->FindVar("sv_maxupdaterate");
	static sdk::ConVar* lerp = ctx::convar->FindVar("cl_interp");
	static sdk::ConVar* cmin = ctx::convar->FindVar("sv_client_min_interp_ratio");
	static sdk::ConVar* cmax = ctx::convar->FindVar("sv_client_max_interp_ratio");
	static sdk::ConVar* ratio = ctx::convar->FindVar("cl_interp_ratio");

	float lerpurmom = lerp->GetFloat();
	float maxupdateurmom = maxupdate->GetFloat();
	int updaterateurmom = updaterate->GetInt();
	float ratiourmom = ratio->GetFloat();
	int sv_maxupdaterate = maxupdate->GetInt();
	int sv_minupdaterate = minupdate->GetInt();
	float cminurmom = cmin->GetFloat();
	float cmaxurmom = cmax->GetFloat();

	if (sv_maxupdaterate && sv_minupdaterate)
		updaterateurmom = maxupdateurmom;

	if (ratiourmom == 0)
		ratiourmom = 1.0f;

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratiourmom = clamp(ratiourmom, cminurmom, cmaxurmom);

	return max(lerpurmom, ratiourmom / updaterateurmom);
}

bool CAimbot::good_backtrack_tick(int tick)
{
	auto nci = ctx::engine->GetNetChannelInfo();

	if (!nci) {
		return false;
	}

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + LerpTime(), 0.f, 1.f);

	float delta_time = correct - (ctx::globals->curtime - TICKS_TO_TIME(tick));

	return fabsf(delta_time) < 0.2f;
}

void CAimbot::lby_backtrack(sdk::CUserCmd *pCmd, sdk::CBaseEntity* pLocal, sdk::CBaseEntity* pEntity)
{
	int index = pEntity->GetIndex();
	float PlayerVel = abs(pEntity->GetVelocity().Length2D());

	bool playermoving;

	if (PlayerVel > 0.f)
		playermoving = true;
	else
		playermoving = false;

	float lby = pEntity->GetLowerBodyYaw();
	static float lby_timer[65];
	static float lby_proxy[65];
	float server_time = pLocal->GetTickBase() * ctx::globals->interval_per_tick;

	if (lby_proxy[index] != pEntity->GetLowerBodyYaw() && playermoving == false)
	{
		lby_timer[index] = 0;
		lby_proxy[index] = pEntity->GetLowerBodyYaw();
	}

	if (playermoving == false)
	{
		if (server_time >= lby_timer[index])
		{
			tick_to_back[index] = pEntity->GetSimTime();
			lby_to_back[index] = pEntity->GetLowerBodyYaw();
			lby_timer[index] = server_time + 1.1;
		}
	}
	else
	{
		tick_to_back[index] = 0;
		lby_timer[index] = 0;
	}

	if (good_backtrack_tick(TIME_TO_TICKS(tick_to_back[index])))
		backtrack_tick[index] = true;
	else
		backtrack_tick[index] = false;
}

#define FSQR(v) (v * v)
void CAimbot::shoot_enemy(sdk::CUserCmd* cmd)
{
	for (int i = 1; i < 65; i++)
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

		auto weapon = reinterpret_cast<sdk::CBaseWeapon*>(ctx::client_ent_list->GetClientEntity(local_player->GetActiveWeaponIndex()));

		if (!weapon)
			continue;

		if (weapon->get_full_info()->WeaponType == 9 || weapon->get_full_info()->WeaponType == 0)
			continue;

		if (weapon->GetLoadedAmmo() == 0)
			continue;

		Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
		Vector head_point = get_hitbox_pos(entity, 0);
		Vector scan_point;

		if (Vars::options.multi_bool)
		{
			if ( Vars::options.acc_type == 1 ) {
				scan_point = smart_baimpoint( entity ); get_hitbox_pos( entity, smart_baim( entity ) );
			}			
			else {
				scan_point = scan_hitpoint( entity ); get_hitbox_pos( entity, scan_hitbox( entity ) );
			}	
		}
		else
		{
			if (Vars::options.acc_type == 1)
				scan_point = get_hitbox_pos(entity, smart_baim(entity));
			else
				scan_point = get_hitbox_pos(entity, scan_hitbox(entity));
		}

		lby_backtrack(cmd, local_player, entity);

		if (Vars::options.aim_bool)
		{
			
			float new_yaw;
			if (accepted_inaccuracy(weapon) > Vars::options.chance_val) //50
			{
				if (Vars::options.acc_type == 0)
				{
					if (autowall->CalculateDamage(local_position, head_point, local_player, entity).damage > Vars::options.damage_val && can_shoot()) //targetp, 15
					{
						if (Vars::options.reverse_bool)
							GLOBAL::should_send_packet = false;

							cmd->viewangles = MATH::NormalizeAngle(UTILS::CalcAngle(local_position, head_point));
							if ( Vars::options.aim_autofire )
								cmd->buttons |= IN_ATTACK;
							

						if (can_shoot())
							shots_fired[entity->GetIndex()]++; //targetp

						if (entity->GetVelocity().Length2D() > 0)
						{
							if (can_shoot() && good_backtrack_tick(TIME_TO_TICKS(entity->GetSimTime())))
								cmd->tick_count = TIME_TO_TICKS(entity->GetSimTime() + LerpTime());
						}
						else
						{
							if (backtrack_tick[entity->GetIndex()])
								cmd->tick_count = TIME_TO_TICKS(tick_to_back[entity->GetIndex()] + LerpTime());
						}
					}
				}
				else 
				{
					if (autowall->CalculateDamage(local_position, scan_point, local_player, entity).damage > Vars::options.damage_val && can_shoot()) //targetp, 15
					{
						if (Vars::options.reverse_bool)
							GLOBAL::should_send_packet = false;

							cmd->viewangles = MATH::NormalizeAngle(UTILS::CalcAngle(local_position, scan_point));
							if ( Vars::options.aim_autofire )
								cmd->buttons |= IN_ATTACK;

						if (can_shoot())
							shots_fired[entity->GetIndex()]++; //targetp

						if (entity->GetVelocity().Length2D() > 0)
						{
							if ( can_shoot( ) && good_backtrack_tick( TIME_TO_TICKS( entity->GetSimTime( ) ) ) )
								cmd->tick_count = TIME_TO_TICKS( entity->GetSimTime( ) + LerpTime( ) );
						}
						else
						{
							if (backtrack_tick[entity->GetIndex()])
								cmd->tick_count = TIME_TO_TICKS(tick_to_back[entity->GetIndex()] + LerpTime());
						}
					}
				}
			}
		}
	}
}

/*bool CAimbot::ValidTick(LagRecord* targetRecord, LagRecord* prevRecord)
{
	if (targetRecord == nullptr)
		return false;

	int tick = TIME_TO_TICKS(targetRecord->m_flSimulationTime);

	sdk::INetChannelInfo* nci = ctx::engine->GetNetChannelInfo();
	float correct = 0.0f;

	correct += nci->GetLatency(FLOW_OUTGOING);

	float m_fLerpTime = LerpTime();
	int lerpTicks = TIME_TO_TICKS(m_fLerpTime);

	correct += TICKS_TO_TIME(lerpTicks);

	int targettick = tick - lerpTicks;
	int predictTickcount = ctx::globals->tickcount + 1 + TIME_TO_TICKS(nci->GetAvgLatency(FLOW_INCOMING) + nci->GetAvgLatency(FLOW_OUTGOING));
	float deltaTime = correct - TICKS_TO_TIME(predictTickcount - targettick);

	if (fabs(deltaTime) > 0.2f)
		// too much difference, can't backtrack here
		return false;
	else if (prevRecord == nullptr)
		return true;

	return (prevRecord->m_vecOrigin - targetRecord->m_vecOrigin).LengthSqr() < 4096;
}*/

struct AimbotTargetIter {
	sdk::CBaseEntity* target;
	int hp;
	float fov;
	float distance;
	float threat;
	int lagticks;
	int predictTicks;
	size_t id;

	AimbotTargetIter() : target(nullptr), hp(1000), fov(10000.0f), distance(1000000.0f), threat(1.0f), id(0), lagticks(1), predictTicks(0) {}

	AimbotTargetIter(sdk::CBaseEntity* pl, int health, float FOV, float dist, float b1gThreat, size_t plID, int lTicks, int pTicks)
	{
		target = pl;
		hp = health;
		fov = FOV;
		distance = dist;
		threat = b1gThreat;
		id = plID;
		lagticks = lTicks;
		predictTicks = pTicks;
	}
};

float CAimbot::accepted_inaccuracy(sdk::CBaseWeapon* weapon) //ayyyyyywareeee
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return 0;

	if (!weapon)
		return 0;

	float hitchance = 101; //lol idk why, its pasted anyway so w/e
	float inaccuracy = weapon->GetInaccuracy();

	if (inaccuracy == 0) 
		inaccuracy = 0.0000001;

	inaccuracy = 1 / inaccuracy;
	hitchance = inaccuracy;

	return hitchance;
}

bool CAimbot::meets_requirements(sdk::CBaseEntity* entity)
{
	//is the aimbot targeted on this enemy
	return true;
}

int CAimbot::select_target()
{
	//finds a target based on distance
	return 0;
}

/*void CAimbot::quick_stop(sdk::CUserCmd* cmd)
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (GetAsyncKeyState(6) && Vars::options.stop_bool)
	{
		cmd->forwardmove = 450;
		aimbot->rotate_movement(UTILS::CalcAngle(Vector(0, 0, 0), local_player->GetVelocity()).y + 180.f, cmd);
	}
}*/

static std::vector<Vector> GetMultiplePointsForHitbox(sdk::CBaseEntity* Local, sdk::CBaseEntity* pBaseEntity, int iHitbox, matrix3x4_t BoneMatrix[])
{
	auto VectorTransform_Wrapper = [](const Vector& in1, const matrix3x4_t &in2, Vector &out)
	{
		auto VectorTransform = [](const float *in1, const matrix3x4_t& in2, float *out)
		{
			auto DotProducts = [](const float *v1, const float *v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	sdk::studiohdr_t* pStudioModel = ctx::model_info->GetStudioModel(pBaseEntity->GetModel());
	sdk::mstudiohitboxset_t* set = pStudioModel->GetHitboxSet(0);
	sdk::mstudiobbox_t *hitbox = set->GetHitbox(iHitbox);

	std::vector<Vector> vecArray;

	Vector max;
	Vector min;
	VectorTransform_Wrapper(hitbox->bbmax, BoneMatrix[hitbox->bone], max);
	VectorTransform_Wrapper(hitbox->bbmin, BoneMatrix[hitbox->bone], min);

	auto center = (min + max) * .5f;

	Vector CurrentAngles = UTILS::CalcAngle(center, Local->GetVecOrigin() + Local->GetViewOffset());

	Vector Forward;
	MATH::AngleVectors(CurrentAngles, &Forward);

	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(-Right.x, -Right.y, Right.z);
	Vector Top(0, 0, 1);
	Vector Bot(0, 0, -1);
	Vector Test(0, 0, -1.2);

	if (!Vars::options.reverse_bool) {
		vecArray.emplace_back(center);
		return vecArray;
	}

	switch (iHitbox) {
	case 0:
		for (auto i = 0; i < 5; ++i)
			vecArray.emplace_back(center);

		vecArray[1] += Top * (hitbox->m_flRadius * (Vars::options.point_val / 100.f));
		vecArray[2] += Right * (hitbox->m_flRadius * (Vars::options.point_val / 100.f));
		vecArray[3] += Left * (hitbox->m_flRadius * (Vars::options.point_val / 100.f));
		vecArray[4] += Test * (hitbox->m_flRadius * (Vars::options.point_val / 100.f));
		break;

	default:
		for (auto i = 0; i < 3; ++i)
			vecArray.emplace_back(center);

		vecArray[1] += Right * (hitbox->group * (Vars::options.body_val / 100.f));
		vecArray[2] += Left * (hitbox->m_flRadius * (Vars::options.body_val / 100.f));
		break;
	}
	return vecArray;
}

int CAimbot::scan_hitbox( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return -1;

	Vector local_position = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );
	static int hitboxes [ ] = { 0, 1, 2, 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 11, 13, 8, 10, 12 };
	mostDamage = Vars::options.damage_val;

	for ( int i = 0; i < ARRAYSIZE( hitboxes ); i++ ) {
		Vector point = get_hitbox_pos( entity, hitboxes [ i ] );
		int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;
		if ( damage > mostDamage ) {
			bestHitbox = hitboxes [ i ];
			mostDamage = damage;
		}
		if ( damage >= entity->GetHealth( ) ) {
			bestHitbox = hitboxes [ i ];
			mostDamage = damage;
			break;
		}
	}
	return bestHitbox;
}

int CAimbot::smart_baim( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return -1;

	Vector local_position = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );

	static int hitboxes [ ] = { 0, 2, 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 11, 8, 1, 13, 10, 12 };
	static int baim_hitboxes [ ] = { 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 11, 13, 10, 12 };
	mostDamage = Vars::options.damage_val;

	if ( entity->GetVelocity( ).Length2D( ) > 36.f || backtrack_tick [ entity->GetIndex( ) ] ) {
		for ( int i = 0; i < ARRAYSIZE( hitboxes ); i++ ) {
			Vector point = get_hitbox_pos( entity, hitboxes [ i ] );
			int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;
			if ( damage > mostDamage ) {
				bestHitbox = hitboxes [ i ];
				mostDamage = damage;
			}
			if ( damage >= entity->GetHealth( ) ) {
				bestHitbox = hitboxes [ i ];
				mostDamage = damage;
				break;
			}
		}
	}
	else {
		for ( int i = 0; i < ARRAYSIZE( baim_hitboxes ); i++ ) {
			Vector point = get_hitbox_pos( entity, baim_hitboxes [ i ] );
			int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;
			if ( damage > mostDamage ) {
				bestHitbox = baim_hitboxes [ i ];
				mostDamage = damage;
			}
			if ( damage >= entity->GetHealth( ) ) {
				bestHitbox = baim_hitboxes [ i ];
				mostDamage = damage;
				break;
			}
		}
	}
	return bestHitbox;
}

Vector CAimbot::scan_hitpoint( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return get_hitbox_pos( entity, 0 );

	Vector local_position = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );
	Vector vector_best_point;
	static int hitboxes [ ] = { 0, 1, 2, 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 11, 13, 8, 10, 12 };
	mostDamage = Vars::options.damage_val;

	matrix3x4_t matrix [ 128 ];
	if ( !entity->SetupBones( matrix, 128, 256, 0 ) )
		return get_hitbox_pos( entity, 0 );

	for ( int i = 0; i < ARRAYSIZE( hitboxes ); i++ ) {
		for ( auto point : GetMultiplePointsForHitbox( local_player, entity, hitboxes [ i ], matrix ) ) {

			int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;

			if ( !damage ) continue;

			if ( damage > mostDamage ) {
				mostDamage = damage;
				vector_best_point = point;
				bestHitbox = hitboxes [ i ];
			}
			if ( damage >= entity->GetHealth( ) ) {
				mostDamage = damage;
				vector_best_point = point;
				bestHitbox = hitboxes [ i ];
			}
		}
		/*Vector point = get_hitbox_pos(entity, hitboxes[i]);
		int damage = autowall->CalculateDamage(local_position, point, local_player, entity).damage;
		if (damage > mostDamage)
		{
		bestHitbox = hitboxes[i];
		mostDamage = damage;
		}
		if (damage >= entity->GetHealth())
		{
		bestHitbox = hitboxes[i];
		mostDamage = damage;
		break;
		}*/
	}
	return vector_best_point;
}

Vector CAimbot::smart_baimpoint( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return get_hitbox_pos( entity, 0 );

	Vector local_position = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );
	Vector vector_best_point;

	static int hitboxes [ ] = { 0,  2, 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 8, 1, 11, 13,  10, 12 };
	static int baim_hitboxes [ ] = { 7, 6, 5, 4, 3, 18, 19, 16, 17, 9, 11, 13, 10, 12 };
	mostDamage = Vars::options.damage_val;

	matrix3x4_t matrix [ 128 ];
	if ( !entity->SetupBones( matrix, 128, 256, 0 ) )
		return get_hitbox_pos( entity, 0 );

	if ( entity->GetVelocity( ).Length2D( ) > 36.f || backtrack_tick [ entity->GetIndex( ) ] ) {
		for ( int i = 0; i < ARRAYSIZE( hitboxes ); i++ ) {
			for ( auto point : GetMultiplePointsForHitbox( local_player, entity, hitboxes [ i ], matrix ) ) {

				int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;

				if ( !damage ) continue;

				if ( damage > mostDamage ) {
					mostDamage = damage;
					vector_best_point = point;
					bestHitbox = hitboxes [ i ];
				}
				if ( damage >= entity->GetHealth( ) ) {
					mostDamage = damage;
					vector_best_point = point;
					bestHitbox = hitboxes [ i ];
				}
			}
		}
	}
	else {
		for ( int i = 0; i < ARRAYSIZE( baim_hitboxes ); i++ ) {
			for ( auto point : GetMultiplePointsForHitbox( local_player, entity, baim_hitboxes [ i ], matrix ) ) {

				int damage = autowall->CalculateDamage( local_position, point, local_player, entity ).damage;

				if ( !damage ) continue;

				if ( damage > mostDamage ) {
					mostDamage = damage;
					vector_best_point = point;
					bestHitbox = baim_hitboxes [ i ];
				}
				if ( damage >= entity->GetHealth( ) ) {
					mostDamage = damage;
					vector_best_point = point;
					bestHitbox = baim_hitboxes [ i ];
				}
			}
		}
	}
	return vector_best_point;
}

bool CAimbot::can_shoot()
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player || local_player->GetHealth() <= 0)
		return false;

	auto weapon = reinterpret_cast<sdk::CBaseWeapon*>(ctx::client_ent_list->GetClientEntity(local_player->GetActiveWeaponIndex()));

	if (!weapon)
		return false;

	return (weapon->GetNextPrimaryAttack() < UTILS::GetCurtime()) && (local_player->GetNextAttack() < UTILS::GetCurtime());
}

sdk::mstudiobbox_t* CAimbot::get_hitbox(sdk::CBaseEntity* entity, int hitbox_index)
{
	if (entity->GetIsDormant() || entity->GetHealth() <= 0)
		return NULL;

	const auto pModel = entity->GetModel();
	if (!pModel)
		return NULL;

	auto pStudioHdr = ctx::model_info->GetStudioModel(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->GetHitboxSet(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		return NULL;

	return pSet->GetHitbox(hitbox_index);
}

Vector CAimbot::get_hitbox_pos(sdk::CBaseEntity* entity, int hitbox_id)
{
	auto hitbox = get_hitbox(entity, hitbox_id);
	if (!hitbox)
		return Vector(0, 0, 0);

	auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

	Vector bbmin, bbmax;
	MATH::VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
	MATH::VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}

void CAimbot::fix_recoil(sdk::CUserCmd* cmd)
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return;

	float RecoilScale = ctx::convar->FindVar("weapon_recoil_scale")->GetFloat(); //huhuuhhuuh
	cmd->viewangles -= *local_player->GetPunchAngles() * 2.f;
}

int CAimbot::get_damage(Vector position) //not needed, thanks autowall!
{
	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());

	if (!local_player)
		return 0;

	auto weapon = reinterpret_cast<sdk::CBaseWeapon*>(ctx::client_ent_list->GetClientEntity(local_player->GetActiveWeaponIndex()));

	if (!weapon)
		return 0;

	sdk::trace_t trace;
	sdk::Ray_t ray;
	sdk::CTraceWorldOnly filter;
	ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), position);

	ctx::trace->TraceRay(ray, MASK_ALL, (sdk::ITraceFilter*)&filter, &trace);

	if (trace.flFraction == 1.f)
	{
		auto weapon_info = weapon->get_full_info();
		if (!weapon_info)
			return -1;

		return weapon_info->iDamage;
		return 1;
	}
	else
		return 0;
}

CAimbot* aimbot = new CAimbot();