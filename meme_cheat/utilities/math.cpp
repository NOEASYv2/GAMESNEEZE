#include "../../includes.h"

#include "qangle.h"
#include "math.h"

typedef float vec_t;

namespace MATH {

	void VectorAngles( const Vector& forward, Vector &angles ) {
		float tmp, yaw, pitch;

		if ( forward [ 1 ] == 0 && forward [ 0 ] == 0 ) {
			yaw = 0;
			if ( forward [ 2 ] > 0 )
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = ( atan2( forward [ 1 ], forward [ 0 ] ) * 180 / M_PI );
			if ( yaw < 0 )
				yaw += 360;

			tmp = sqrt( forward [ 0 ] * forward [ 0 ] + forward [ 1 ] * forward [ 1 ] );
			pitch = ( atan2( -forward [ 2 ], tmp ) * 180 / M_PI );
			if ( pitch < 0 )
				pitch += 360;
		}

		angles [ 0 ] = pitch;
		angles [ 1 ] = yaw;
		angles [ 2 ] = 0;
	}
	void rotate_point( Vector2D& point, Vector2D origin, bool clockwise, float angle ) {
		Vector2D delta = point - origin;
		Vector2D rotated( delta.x * cos( angle ) - delta.y * sin( angle ), delta.x * sin( angle ) + delta.y * cos( angle ) );

		if ( clockwise ) {
			rotated = Vector2D( delta.x * cos( angle ) - delta.y * sin( angle ), delta.x * sin( angle ) + delta.y * cos( angle ) );
		}
		else {
			rotated = Vector2D( delta.x * sin( angle ) - delta.y * cos( angle ), delta.x * cos( angle ) + delta.y * sin( angle ) );
		}

		point = rotated + origin;
	}
	/*inline void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result)
	{
	CHECK_VALID(a);
	CHECK_VALID(b);

	Assert(&a != &result);
	Assert(&b != &result);

	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	}*/
	void CrossProduct( const float* v1, const float* v2, float* cross ) {
		cross [ 0 ] = v1 [ 1 ] * v2 [ 2 ] - v1 [ 2 ] * v2 [ 1 ];
		cross [ 1 ] = v1 [ 2 ] * v2 [ 0 ] - v1 [ 0 ] * v2 [ 2 ];
		cross [ 2 ] = v1 [ 0 ] * v2 [ 1 ] - v1 [ 1 ] * v2 [ 0 ];
	}
	Vector NormalizeAngle( Vector angle ) {
		while ( angle.x > 89.f ) {
			angle.x -= 180.f;
		}
		while ( angle.x < -89.f ) {
			angle.x += 180.f;
		}
		if ( angle.y > 180 ) {
			angle.y -= ( round( angle.y / 360 ) * 360.f );
		}
		else if ( angle.y < -180 ) {
			angle.y += ( round( angle.y / 360 ) * -360.f );
		}
		if ( ( angle.z > 50 ) || ( angle.z < 50 ) ) {
			angle.z = 0;
		}
		return angle;
	}
	float NormalizeYaw( float yaw ) {
		if ( yaw > 180 ) {
			yaw -= ( round( yaw / 360 ) * 360.f );
		}
		else if ( yaw < -180 ) {
			yaw += ( round( yaw / 360 ) * -360.f );
		}
		return yaw;
	}
	void NormalizeNum( Vector &vIn, Vector &vOut ) {
		float flLen = vIn.Length( );
		if ( flLen == 0 ) {
			vOut.Init( 0, 0, 1 );
			return;
		}
		flLen = 1 / flLen;
		vOut.Init( vIn.x * flLen, vIn.y * flLen, vIn.z * flLen );
	}
	void inline SinCos( float radians, float *sine, float *cosine ) {
		*sine = sin( radians );
		*cosine = cos( radians );

	}
	void VectorSubtract( const Vector& a, const Vector& b, Vector& c ) {
		c.x = a.x - b.x;
		c.y = a.y - b.y;
		c.z = a.z - b.z;
	}
	inline void VectorCrossProduct( const Vector& a, const Vector& b, Vector& result )

	{

		CHECK_VALID( a );

		CHECK_VALID( b );

		Assert( &a != &result );

		Assert( &b != &result );

		result.x = a.y*b.z - a.z*b.y;

		result.y = a.z*b.x - a.x*b.z;

		result.z = a.x*b.y - a.y*b.x;

	}
	void AngleVectors( const Vector &angles, Vector *forward ) {
		float	sp, sy, cp, cy;

		SinCos( DEG2RAD( angles [ 1 ] ), &sy, &cy );
		SinCos( DEG2RAD( angles [ 0 ] ), &sp, &cp );

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	void AngleVectors( Vector &angles, Vector *forward ) {
		float	sp, sy, cp, cy;

		SinCos( DEG2RAD( angles [ 1 ] ), &sy, &cy );
		SinCos( DEG2RAD( angles [ 0 ] ), &sp, &cp );

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	void AngleVectors( const QAngle& angles, Vector* forward ) {
		float sp, sy, cp, cy;

		SinCos( DEG2RAD( angles.yaw ), &sy, &cy );
		SinCos( DEG2RAD( angles.pitch ), &sp, &cp );

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	__forceinline float DotProduct( const float *a, const float *b ) {
		return a [ 0 ] * b [ 0 ] + a [ 1 ] * b [ 1 ] + a [ 2 ] * b [ 2 ];
	}
	Vector RotateVectorYaw( Vector origin, float angle, Vector point ) {
		float s;
		float c;
		SinCos( DEG2RAD( angle ), &s, &c );

		// translate point back to origin:
		point.x -= origin.x;
		point.y -= origin.y;

		// rotate point
		float xnew = point.x * c - point.y * s;
		float ynew = point.x * s + point.y * c;

		// translate point back:
		point.x = xnew + origin.x;
		point.y = ynew + origin.y;
		return point;
	}
	void VectorTransform( const float *in1, const matrix3x4_t& in2, float *out ) {
		out [ 0 ] = DotProduct( in1, in2 [ 0 ] ) + in2 [ 0 ] [ 3 ];
		out [ 1 ] = DotProduct( in1, in2 [ 1 ] ) + in2 [ 1 ] [ 3 ];
		out [ 2 ] = DotProduct( in1, in2 [ 2 ] ) + in2 [ 2 ] [ 3 ];
	}
	void VectorTransform( const Vector& in1, const matrix3x4_t& in2, Vector& out ) {
		VectorTransform( &in1.x, in2, &out.x );
	}
	void AngleVectorsA4( const Vector &angles, Vector *forward, Vector *right, Vector *up ) {
		float sr, sp, sy, cr, cp, cy;

		SinCos( DEG2RAD( angles [ 1 ] ), &sy, &cy );
		SinCos( DEG2RAD( angles [ 0 ] ), &sp, &cp );
		SinCos( DEG2RAD( angles [ 2 ] ), &sr, &cr );

		if ( forward ) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if ( right ) {
			right->x = ( -1 * sr*sp*cy + -1 * cr*-sy );
			right->y = ( -1 * sr*sp*sy + -1 * cr*cy );
			right->z = -1 * sr*cp;
		}

		if ( up ) {
			up->x = ( cr*sp*cy + -sr * -sy );
			up->y = ( cr*sp*sy + -sr * cy );
			up->z = cr * cp;
		}
	}
	float CalcAngle2D( Vector2D src, Vector2D dst ) {
		float angle;
		VectorAngle2D( dst - src, angle );
		return angle;
	}

	void VectorAngles2( const Vector& forward, QAngle& angles ) {
		if ( forward [ 1 ] == 0.0f && forward [ 0 ] == 0.0f ) {
			angles [ 0 ] = ( forward [ 2 ] > 0.0f ) ? 270.0f : 90.0f;
			angles [ 1 ] = 0.0f;
		}
		else {
			angles [ 0 ] = atan2( -forward [ 2 ], forward.Length2D( ) ) * -180 / M_PI;
			angles [ 1 ] = atan2( forward [ 1 ], forward [ 0 ] ) * 180 / M_PI;

			if ( angles [ 1 ] > 90 )
				angles [ 1 ] -= 180;
			else if ( angles [ 1 ] < 90 )
				angles [ 1 ] += 180;
			else if ( angles [ 1 ] == 90 )
				angles [ 1 ] = 0;
		}

		angles [ 2 ] = 0.0f;
	}

	QAngle CalcAngle( Vector src, Vector dst ) {
		QAngle angles;
		Vector delta = src - dst;
		VectorAngles2( delta, angles );
		delta.Normalized( );
		return angles;
	}
	void CalcAngle( Vector src, Vector dst, Vector &angles ) {
		Vector delta = src - dst;
		double hyp = delta.Length2D( );
		angles.y = ( atan( delta.y / delta.x ) * 57.295779513082f );
		angles.x = ( vec_t ) ( atan( delta.z / hyp ) * 57.295779513082f );
		angles [ 2 ] = 0.00;

		if ( delta.x >= 0.0 )
			angles.y += 180.0f;
	}

	void VectorAngle2D( const Vector2D &direction, float &angle ) {
		angle = RAD2DEG( std::atan2( direction.y, direction.x ) ) + 90.f;
	}
	void AngleVectors2D( float angle, Vector2D &forward ) {
		angle = DEG2RAD( angle );
		Vector2D slope( sin( angle ), -cos( angle ) );
		forward = slope;
	}

	/*
	void draw_angle_line2d(const Vector2D center, const float yaw, const float
	line_distance_from_center, const float line_length, const CColor line_color)
	{
	const auto start_position = get_rotated_screen_position(center, yaw, line_distance_from_center);
	const auto end_position = get_rotated_screen_position(center, yaw, line_distance_from_center + line_length);

	IRender->DrawLine(start_position.x, start_position.y, end_position.x, end_position.y, line_color);
	}
	Vector2D get_rotated_screen_position(Vector2D center, float yaw, float distance)
	{
	Vector2D slope = { sin(DEG2RAD(yaw)), -cos(DEG2RAD(yaw)) }; // x = run, y = rise
	Vector2D direction = slope;
	return center + (slope * distance);
	}
	*/
}