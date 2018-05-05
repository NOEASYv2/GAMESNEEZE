#pragma once

typedef __int16					int16;
typedef unsigned __int16		uint16;
typedef __int32					int32;
typedef unsigned __int32		uint32;
typedef __int64					int64;
typedef unsigned __int64		uint64;

inline float BitsToFloat(uint32 i)
{
	union Convertor_t
	{
		float f;
		unsigned long ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

#define FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN

struct matrix3x4_t
{
public:

	matrix3x4_t() {}

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void SetOrigin(Vector const & p)
	{
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate(void)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_flMatVal[i][j] = VEC_T_NAN;
			}
		}
	}

	float *operator[](int i) { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	const float *operator[](int i) const { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	float *Base() { return &m_flMatVal[0][0]; }
	const float *Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];

	/*matrix3x4_t(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	void Pad();
	void Pad2();

	void Init
	(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);


	void Init(void*);

	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}


	inline float *Base()
	{
		return &m[0][0];
	}

	inline const float *Base() const
	{
		return &m[0][0];
	}

	inline Vector matrix3x4_t::operator*(const Vector &vVec) const
	{
		Vector vRet;
		vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
		vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
		vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

		return vRet;
	}

	matrix3x4_t& matrix3x4_t::operator=(const matrix3x4_t &mOther)
	{
		m[0][0] = mOther.m[0][0];
		m[0][1] = mOther.m[0][1];
		m[0][2] = mOther.m[0][2];
		m[0][3] = mOther.m[0][3];

		m[1][0] = mOther.m[1][0];
		m[1][1] = mOther.m[1][1];
		m[1][2] = mOther.m[1][2];
		m[1][3] = mOther.m[1][3];

		m[2][0] = mOther.m[2][0];
		m[2][1] = mOther.m[2][1];
		m[2][2] = mOther.m[2][2];
		m[2][3] = mOther.m[2][3];

		m[3][0] = mOther.m[3][0];
		m[3][1] = mOther.m[3][1];
		m[3][2] = mOther.m[3][2];
		m[3][3] = mOther.m[3][3];

		return *this;
	}

	bool matrix3x4_t::operator==(const matrix3x4_t& src) const
	{
		return !memcmp(src.m, m, sizeof(m));
	}

	void matrix3x4_t::MatrixMul(const matrix3x4_t &vm, matrix3x4_t &out) const
	{
		out.Init(
			m[0][0] * vm.m[0][0] + m[0][1] * vm.m[1][0] + m[0][2] * vm.m[2][0] + m[0][3] * vm.m[3][0],
			m[0][0] * vm.m[0][1] + m[0][1] * vm.m[1][1] + m[0][2] * vm.m[2][1] + m[0][3] * vm.m[3][1],
			m[0][0] * vm.m[0][2] + m[0][1] * vm.m[1][2] + m[0][2] * vm.m[2][2] + m[0][3] * vm.m[3][2],
			m[0][0] * vm.m[0][3] + m[0][1] * vm.m[1][3] + m[0][2] * vm.m[2][3] + m[0][3] * vm.m[3][3],

			m[1][0] * vm.m[0][0] + m[1][1] * vm.m[1][0] + m[1][2] * vm.m[2][0] + m[1][3] * vm.m[3][0],
			m[1][0] * vm.m[0][1] + m[1][1] * vm.m[1][1] + m[1][2] * vm.m[2][1] + m[1][3] * vm.m[3][1],
			m[1][0] * vm.m[0][2] + m[1][1] * vm.m[1][2] + m[1][2] * vm.m[2][2] + m[1][3] * vm.m[3][2],
			m[1][0] * vm.m[0][3] + m[1][1] * vm.m[1][3] + m[1][2] * vm.m[2][3] + m[1][3] * vm.m[3][3],

			m[2][0] * vm.m[0][0] + m[2][1] * vm.m[1][0] + m[2][2] * vm.m[2][0] + m[2][3] * vm.m[3][0],
			m[2][0] * vm.m[0][1] + m[2][1] * vm.m[1][1] + m[2][2] * vm.m[2][1] + m[2][3] * vm.m[3][1],
			m[2][0] * vm.m[0][2] + m[2][1] * vm.m[1][2] + m[2][2] * vm.m[2][2] + m[2][3] * vm.m[3][2],
			m[2][0] * vm.m[0][3] + m[2][1] * vm.m[1][3] + m[2][2] * vm.m[2][3] + m[2][3] * vm.m[3][3],

			m[3][0] * vm.m[0][0] + m[3][1] * vm.m[1][0] + m[3][2] * vm.m[2][0] + m[3][3] * vm.m[3][0],
			m[3][0] * vm.m[0][1] + m[3][1] * vm.m[1][1] + m[3][2] * vm.m[2][1] + m[3][3] * vm.m[3][1],
			m[3][0] * vm.m[0][2] + m[3][1] * vm.m[1][2] + m[3][2] * vm.m[2][2] + m[3][3] * vm.m[3][2],
			m[3][0] * vm.m[0][3] + m[3][1] * vm.m[1][3] + m[3][2] * vm.m[2][3] + m[3][3] * vm.m[3][3]
		);
	}

	matrix3x4_t matrix3x4_t::operator*(const matrix3x4_t &vm) const
	{
		matrix3x4_t ret;
		MatrixMul(vm, ret);
		return ret;
	}

	inline matrix3x4_t matrix3x4_t::operator+(const matrix3x4_t &other) const
	{
		matrix3x4_t ret;
		for (int i = 0; i < 16; i++)
		{
			((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
		}
		return ret;
	}*/

public:
	float m[4][4];
};

typedef float VMatrix[3][4];