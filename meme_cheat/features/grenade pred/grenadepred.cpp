#include "grenadepred.h"

//cba pasting this shit i guess someone else can LUUL

void grenade_prediction::Tick( int buttons ) {
}
void grenade_prediction::View( sdk::CViewSetup* setup ) {
}

void grenade_prediction::Paint( ) {
}
static const constexpr auto PIRAD1 = 0.01745329251f;
void angle_vectors2( const Vector &angles, Vector *forward, Vector *right, Vector *up ) {
}
void grenade_prediction::Setup( Vector& vecSrc, Vector& vecThrow, QAngle viewangles ) {
}

void grenade_prediction::Simulate( sdk::CViewSetup* setup ) {
}

int grenade_prediction::Step( Vector& vecSrc, Vector& vecThrow, int tick, float interval ) {
	return 420;
}


bool grenade_prediction::CheckDetonate( const Vector& vecThrow, const sdk::trace_t& tr, int tick, float interval ) {
	return false;
}

void grenade_prediction::TraceHull( Vector& src, Vector& end, sdk::trace_t& tr ) {
}

void grenade_prediction::AddGravityMove( Vector& move, Vector& vel, float frametime, bool onground ) {
}

void grenade_prediction::PushEntity( Vector& src, const Vector& move, sdk::trace_t& tr ) {
}

void grenade_prediction::ResolveFlyCollisionCustom( sdk::trace_t& tr, Vector& vecVelocity, float interval ) {
}

int grenade_prediction::PhysicsClipVelocity( const Vector& in, const Vector& normal, Vector& out, float overbounce ) {
	return 69;
}