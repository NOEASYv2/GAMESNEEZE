#pragma once

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
	class CBaseWeapon;
}

class CAimbot
{
public:
	void shoot_enemy(sdk::CUserCmd* cmd);
	bool good_backtrack_tick(int tick);
	void lby_backtrack(sdk::CUserCmd * pCmd, sdk::CBaseEntity * pLocal, sdk::CBaseEntity * pEntity);
	//bool ValidTick(LagRecord * targetRecord, LagRecord * prevRecord);
	//void GetBestTarget(sdk::CBaseEntity *& targetPlayer, Vector & targetVector, LagRecord *& targetRecord);
	float accepted_inaccuracy(sdk::CBaseWeapon * weapon);
	Vector smart_baimpoint(sdk::CBaseEntity * entity);
	bool can_shoot();
	void fix_recoil(sdk::CUserCmd* cmd);
	void rotate_movement(float yaw, sdk::CUserCmd * cmd);
	Vector get_hitbox_pos(sdk::CBaseEntity* entity, int hitbox_id);
private:
	int select_target();
	void quick_stop(sdk::CUserCmd * cmd);
	int scan_hitbox(sdk::CBaseEntity * entity); //int
	Vector scan_hitpoint(sdk::CBaseEntity * entity);
	int smart_baim(sdk::CBaseEntity * entity);
	sdk::mstudiobbox_t* get_hitbox(sdk::CBaseEntity* entity, int hitbox_index);
	bool meets_requirements(sdk::CBaseEntity* entity);
	int get_damage(Vector position);
	bool spread_limit(sdk::CBaseWeapon* weapon);
	float seedchance(Vector Point);
};

extern CAimbot* aimbot;