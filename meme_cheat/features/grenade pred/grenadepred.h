#pragma once
#include "../../utilities/singleton.h"
#include "../../includes.h"

#include "../../sdk/CInput.h"
#include "../../sdk/CClientState.h"
#include "../../sdk/IClient.h"
#include "../../sdk/CPanel.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/CGlowObjectManager.h"
#include "../../sdk/IVDebugOverlay.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CBaseAnimState.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/RecvData.h"
#include "../../sdk/ModelInfo.h"
#include "../../sdk/ModelRender.h"
#include "../../sdk/RenderView.h"
#include "../../sdk/NetChannel.h"
#include "../../sdk/CIncomingSequences.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CViewSetup.h"
#include "../../sdk/CGlobalVars.h"

enum ACT {
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class grenade_prediction
	: public Singleton<grenade_prediction> {
private:
	std::vector<Vector> path;

	int type = 0;
	int act = 0;

	void Setup( Vector& vecSrc, Vector& vecThrow, QAngle viewangles );
	void Simulate( sdk::CViewSetup* setup );

	int Step( Vector& vecSrc, Vector& vecThrow, int tick, float interval );
	bool CheckDetonate( const Vector& vecThrow, const sdk::trace_t& tr, int tick, float interval );

	void TraceHull( Vector& src, Vector& end, sdk::trace_t& tr );
	void AddGravityMove( Vector& move, Vector& vel, float frametime, bool onground );
	void PushEntity( Vector& src, const Vector& move, sdk::trace_t& tr );
	void ResolveFlyCollisionCustom( sdk::trace_t& tr, Vector& vecVelocity, float interval );
	int PhysicsClipVelocity( const Vector& in, const Vector& normal, Vector& out, float overbounce );
public:
	void Tick( int buttons );
	void View( sdk::CViewSetup* setup );
	void Paint( );
};