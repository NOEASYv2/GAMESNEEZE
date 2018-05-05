#pragma once

#include <math.h>

#define M_PI 3.14159265358979323846
#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

class Vector
{
public:
	float x, y, z;
	inline void Init(float ix, float iy, float iz)
	{
		x = ix; y = iy; z = iz;
	}
	Vector VectorCrossProduct(const Vector &a, const Vector &b) const
	{
		return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}
	Vector() { x = 0; y = 0; z = 0; };
	Vector(float X, float Y, float Z) { x = X; y = Y; z = Z; };

	float operator[](int i) const { if (i == 0) return x; if (i == 1) return y; return z; };
	float& operator[](int i) { if (i == 0) return x; if (i == 1) return y; return z; };

	bool operator==(const Vector& v) { return true; }
	bool operator!=(const Vector& v) { return true; }

	inline Vector operator-(const Vector& v) { return Vector(x - v.x, y - v.y, z - v.z); }
	inline Vector operator+(const Vector& v) { return Vector(x + v.x, y + v.y, z + v.z); }
	inline Vector operator*(const Vector& v) { return Vector(x * v.x, y * v.y, z * v.z); }
	inline Vector operator/(const Vector& v) { return Vector(x / v.x, y / v.y, z / v.z); }
	inline Vector operator*(const int n) { return Vector(x*n, y*n, z*n); }
	inline Vector operator*(const float n) { return Vector(x*n, y*n, z*n); }
	inline Vector operator/(const int n) { return Vector(x / n, y / n, z / n); }
	inline Vector operator/(const float n) { return Vector(x / n, y / n, z / n); }


	inline Vector operator-() { return Vector(-x, -y, -z); }

	inline Vector& operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
	inline Vector& operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	inline Vector& operator*=(const Vector &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	inline Vector& operator/=(const Vector &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

	float LengthSqr(void) { return (x*x + y*y + z*z); }
	float Length(void) { return sqrt(x*x + y*y + z*z); }

	inline float Length2D() const
	{
		return sqrt((x * x) + (y * y));
	}

	//T must be between 0 and 1
	Vector lerp(Vector target, float t)
	{
		return *this * (1 - t) + target * t;
	}
#define deg(a) a * 57.295779513082
	inline Vector Angle( Vector* up = 0 ) {
		if ( !x && !y )
			return Vector( 0, 0, 0 );

		float roll = 0;

		if ( up ) {
			Vector left = ( *up ).Cross( *this );

			roll = deg( atan2f( left.z, ( left.y * x ) - ( left.x * y ) ) );
		}

		return Vector( deg( atan2f( -z, sqrtf( x*x + y * y ) ) ), deg( atan2f( y, x ) ), roll );
	}
	void lerpme(Vector target, float t)
	{
		*this = *this * (1 - t) + target * t;
	}
	inline float Dot(const Vector &e) const
	{
		return (x * e.x) + (y * e.y) + (z * e.z);
	}
	float DistTo(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	float DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}
	Vector& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}
	/*Vector Cross(const Vector& vOther) const
	{
		Vector res;
		VectorCrossProduct(*this, vOther, res);
		return res;
	}*/
	void crossproduct(Vector v1, Vector v2, Vector cross_p) const //ijk = xyz
	{
		cross_p.x = (v1.y*v2.z) - (v1.z*v2.y); //i
		cross_p.y = -((v1.x*v2.z) - (v1.z*v2.x)); //j
		cross_p.z = (v1.x*v2.y) - (v1.y*v2.x); //k
	}
	Vector Cross(const Vector& vOther) const
	{
		Vector res;
		crossproduct(*this, vOther, res);
		return res;
	}
	float NormalizeInPlace()
	{
		Vector& v = *this;

		float iradius = 1.f / (this->LengthSqr() + 1.192092896e-07F); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
		return iradius;
	}
	Vector Normalized() const
	{
		Vector res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		return res;
	}
};
