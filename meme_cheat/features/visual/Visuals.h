#pragma once

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
}

class CVisuals
{
public:
	struct damage_indicator_t {
		int dmg;
		bool initializes;
		float earse_time;
		float last_update;
		sdk::CBaseEntity * player;
		Vector Position;
	};
	std::vector<damage_indicator_t> dmg_indicator;

	struct loginfo {
		loginfo( std::string text,
			CColor color,
			float time ) {
			this->text = text;
			this->color = color;
			this->time = time;
		}

		std::string text;
		CColor color;
		float time;
	};
	std::deque<loginfo>events;

	struct Footstep {
		long expiration;
		int entityId;
		Vector position;
	};

	std::vector<Footstep> footsteps;

	struct ESPBox {
		float x, y, w, h;
	};
	RECT GetBBox( sdk::CBaseEntity * entity, Vector pointstransf [ ] );
	void Draw();
	void ClientDraw();
	void OffScreen(sdk::CBaseEntity * entity);
	void AsusWalls( );
	void AsusProps( );
	void ShowSpread( );
	void DrawBulletBeams();
	void EmitSound( int iEntIndex, const char * pSample );
private:
	bool GetBox( sdk::CBaseEntity * pEntity, ESPBox & result );
	void Hitmarker();
	void DrawBox(sdk::CBaseEntity* entity, CColor color);
	void localDrawBox( sdk::CBaseEntity * entity, CColor color );
	void NewBox(sdk::CBaseEntity * entity, CColor color);
	void DrawCorners(sdk::CBaseEntity * entity, CColor color);
	void DrawName(sdk::CBaseEntity* entity, CColor color, int index);
	void DrawAmmo( sdk::CBaseEntity * entity, CColor color, int index );
	void DrawWeapon(sdk::CBaseEntity * entity, CColor color, int index);
	void DrawResolveMode( sdk::CBaseEntity * entity, CColor color, int index );
	void DrawSounds( );
	void CollectFootstep( int iEntIndex, const char * pSample );
	void DrawHealth(sdk::CBaseEntity * entity, CColor color, CColor dormant);
	void DrawAmmo(sdk::CBaseEntity * entity, CColor color, CColor dormant);
	float resolve_distance(Vector src, Vector dest);
	void DrawDistance(sdk::CBaseEntity * entity, CColor color);
	void DrawFlags( sdk::CBaseEntity * entity, CColor color, CColor alt );
	void DrawAnimations( sdk::CBaseEntity * entity, CColor color, CColor alt );
	void DrawInfo(sdk::CBaseEntity * entity, CColor color, CColor alt);
	void DrawSpectators( sdk::CBaseEntity * local );
	void DrawActs( sdk::CBaseEntity * entity, CColor color, CColor alt );
	void DrawSkeleton(sdk::CBaseEntity * entity, CColor color);
	void DrawBomb(sdk::CBaseEntity* entity);
	void DrawCrosshair();
	void DrawDamageIndicator( );
	void DrawIndicator();
	void DrawLogs( );
	void DrawDirection(const Vector & origin);
	void ModulateWorld();
	void DrawBacktrack( );
	void DrawHitmarker();
	void DrawAntiAimSides();
	void DrawBorderLines();
	void DrawBarrel(sdk::CBaseEntity * entity);
public:
	std::vector<std::pair<int, float>>				Entities;
	std::deque<UTILS::BulletImpact_t>				Impacts;
};

extern CVisuals* visuals;