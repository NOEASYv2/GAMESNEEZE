#pragma once
#include "../utilities/qangle.h"
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

typedef float vec_t;

namespace MATH {
	void VectorAngles( const Vector& forward, Vector &angles );
	Vector NormalizeAngle( Vector angle );
	void NormalizeNum( Vector &vIn, Vector &vOut );
	void VectorSubtract( const Vector& a, const Vector& b, Vector& c );
	void AngleVectorsA4( const Vector &angles, Vector *forward, Vector *right, Vector *up );
	float NormalizeYaw( float yaw );
	void inline SinCos( float radians, float *sine, float *cosine );
	void AngleVectors( const Vector &angles, Vector *forward );
	void AngleVectors( const QAngle & angles, Vector * forward );
	__forceinline float DotProduct( const float *a, const float *b );
	void VectorTransform( const float *in1, const matrix3x4_t& in2, float *out );
	void VectorTransform( const Vector& in1, const matrix3x4_t& in2, Vector& out );
	void rotate_point( Vector2D& point, Vector2D origin, bool clockwise, float angle );
	Vector RotateVectorYaw( Vector origin, float angle, Vector point );
	float CalcAngle2D( Vector2D src, Vector2D dst );
	QAngle CalcAngle( Vector src, Vector dst );
	void CalcAngle( Vector src, Vector dst, Vector & angles );
	//inline void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result);
	void VectorAngle2D( const Vector2D &direction, float &angle );
	void AngleVectors2D( float angle, Vector2D &forward );
}