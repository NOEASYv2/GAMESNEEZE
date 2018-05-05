#pragma once
#include <math.h>

class Vector2D
{
public:
	float x, y;

	Vector2D() { x = 0; y = 0; };
	Vector2D(float X, float Y) { x = X; y = Y; };
	Vector2D(Vector vec) { x = vec.x; y = vec.y; }

	inline Vector2D operator*(const float n) const { return Vector2D(x*n, y*n); }
	inline Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
	inline Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
	inline void operator+=(const Vector2D& v)
	{ 
		x += v.x;
		y += v.y;
	}
	inline void operator-=(const Vector2D& v)
	{
		x -= v.x;
		y -= v.y;
	}

	bool operator==(const Vector2D& v) const { return (v.x == x && v.y == y); }
	bool operator!=(const Vector2D& v) const { return (v.x != x || v.y != y); }

	inline float length()
	{
		return sqrt((x * x) + (y * y));
	}
};
