#include "../../includes.h"

#include "offsets.h"


namespace OFFSETS
{
	uintptr_t m_iHealth;
	uintptr_t m_fFlags;
	uintptr_t m_vecVelocity;
	uintptr_t m_flLowerBodyYawTarget;
	uintptr_t deadflag;
	uintptr_t m_vecOrigin;
	uintptr_t m_iTeamNum;
	uintptr_t m_nTickBase;
	uintptr_t m_bDormant;
	uintptr_t animstate;
	uintptr_t m_Collision;
	uintptr_t m_angEyeAngles;
	uintptr_t m_flSimulationTime;
	uintptr_t m_vecViewOffset;
	uintptr_t m_dwBoneMatrix;
	uintptr_t m_aimPunchAngle;
	uintptr_t m_viewPunchAngle;
	uintptr_t m_bGunGameImmunity;
	uintptr_t m_nForceBone;
	uintptr_t m_flPoseParameter;
	uintptr_t dwGlowObjectManager;
	uintptr_t m_flNextPrimaryAttack;
	uintptr_t m_flNextAttack;
	uintptr_t m_hActiveWeapon;
	uintptr_t m_ArmorValue;
	uintptr_t m_bHasHelmet;
	uintptr_t m_iObserverMode;
	uintptr_t m_bIsScoped;
	uintptr_t m_iMoveType;
	uintptr_t m_iAccount;
	uintptr_t m_iPlayerC4;
	uintptr_t dwPlayerResource;
	uintptr_t m_iItemDefinitionIndex;

	void InitOffsets()
	{
		m_iHealth = 0xFC;
		m_fFlags = 0x100;
		m_vecVelocity = 0x110;
		m_flLowerBodyYawTarget = 0x39DC;
		deadflag = 0x31C4;
		m_vecOrigin = 0x0134;
		m_viewPunchAngle = 0x3010;
		m_iTeamNum = 0xF0;
		m_nTickBase = 0x3404;
		m_bDormant = 0xE9;
		animstate = 0x3874;
		m_iMoveType = 0x258;
		m_Collision = 0x318;
		m_angEyeAngles = 0xB23C;
		m_flSimulationTime = 0x264;
		m_vecViewOffset = 0x104;
		m_dwBoneMatrix = 0x2698;
		m_aimPunchAngle = 0x301C;
		m_bGunGameImmunity = 0x3894;
		m_nForceBone = 0x267C;
		m_flPoseParameter = 0x2764;
		dwGlowObjectManager = 0x4F9D2B8;
		m_flNextPrimaryAttack = 0x31D8;
		m_flNextAttack = 0x2D60;
		m_hActiveWeapon = 0x2EE8;
		m_ArmorValue = 0xB238;
		m_bHasHelmet = 0xB22C;
		m_iObserverMode = 0x334C;
		m_bIsScoped = 0x387E;
		m_iAccount = 0xB228;
		m_iPlayerC4 = 0x161C;
		dwPlayerResource = 0x2EC3F5C;
		m_iItemDefinitionIndex = 0x2F88;
	}
}