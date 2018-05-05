#pragma once
#include <array>

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
	class CBaseWeapon;
}

struct STickRecord {

	bool operator==( STickRecord &other ) {
		return other.m_flSimulationTime == m_flSimulationTime;
	}

	float m_flVelocity = 0.f;
	Vector m_vecVelocity = Vector( 0, 0, 0 );
	float m_flSimulationTime = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	QAngle m_angEyeAngles = QAngle( 0, 0, 0 );
	std::array<float, 24> m_flPoseParameter = { };
	float m_flCurTime = 0.f;
	int m_nFlags = 0;

	int m_iLayerCount = 0;
	sdk::CAnimationLayer animationLayer [ 15 ];
};

struct Info 
{
	Info() {}

	sdk::CAnimationLayer backup_layer, prev_layer;
	Vector last_lby, inverse, inverse_right, inverse_left, lby, back, left, right, backtrack;
	float stored_simtime, last_move_time, pre_anim_lby;
	int last_ammo;
	bool breaking_lby, reset_state, could_be_faking;
	std::vector<float> unresolved_yaw, lby_deltas;

	bool lby_changed;
	bool could_be_slowmo;
	bool is_moving;
	bool is_jumping;
	bool is_crouching;
	bool lby_updated;
	bool using_fake_angles;
	float last_moving_lby;
	float stored_lby;
	float next_lby_update_time;
	int stored_missed;
};

class CResolver
{
public:
	Info player_info[65];
	void record(sdk::CBaseEntity * entity, float new_yaw);
	void think( sdk::CBaseEntity * entity );
	bool IsAdjustingBalance( sdk::CBaseEntity * player, STickRecord & record, sdk::CAnimationLayer * layer );
	void Experimental2( sdk::CBaseEntity * entity );
	void Experimental( sdk::CBaseEntity * entity );
	void Default( sdk::CBaseEntity * entity );
	void Nospread( sdk::CBaseEntity * entity );
	void resolve(sdk::CBaseEntity* entity);
};

extern CResolver* resolver;