#pragma once

#include "logging.h"

class Vector2D;
namespace sdk
{
	class CUserCmd;
}
namespace UTILS
{
	template<class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi)
	{
		return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
	}

	bool is_point_in_range(Vector2D, Vector2D, Vector2D);
	float GetFraction(float, float, float);
	float GetValueFromFraction(float, float, float);
	unsigned int GetNumberOfDigits(int);
	float RoundToDecimalPlaces(float, int);
	std::string FloatToString(float, int);
	Vector CalcAngle(Vector src, Vector dst);
	float GetCurtime();
	float Lerp(float fraction, float min, float max);

	uint64_t FindSignature(const char* szModule, const char* szSignature);
	DWORD WaitOnModuleHandle(std::string moduleName);
	bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask);
	DWORD FindPattern(std::string moduleName, BYTE* Mask, char* szMask);
	std::uint8_t * pattern_scan( void * module, const char * signature );
	bool ClampLemon(Vector& angles);

	void RotateMovement(float yaw, sdk::CUserCmd* cmd);
}