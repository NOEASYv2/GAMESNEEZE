#pragma once

namespace OFFSETS
{
	extern uintptr_t m_iHealth;
	extern uintptr_t m_fFlags;
	extern uintptr_t m_vecVelocity;
	extern uintptr_t m_flLowerBodyYawTarget;
	extern uintptr_t deadflag;
	extern uintptr_t m_vecOrigin;
	extern uintptr_t m_iTeamNum;
	extern uintptr_t m_nTickBase;
	extern uintptr_t m_bDormant;
	extern uintptr_t animstate;
	extern uintptr_t m_Collision;
	extern uintptr_t m_angEyeAngles;
	extern uintptr_t m_flSimulationTime;
	extern uintptr_t m_vecViewOffset;
	extern uintptr_t m_dwBoneMatrix;
	extern uintptr_t m_aimPunchAngle;
	extern uintptr_t m_viewPunchAngle;
	extern uintptr_t m_bGunGameImmunity;
	extern uintptr_t m_nForceBone;
	extern uintptr_t m_flPoseParameter;
	extern uintptr_t dwGlowObjectManager;
	extern uintptr_t m_flNextPrimaryAttack;
	extern uintptr_t m_flNextAttack;
	extern uintptr_t m_hActiveWeapon;
	extern uintptr_t m_ArmorValue;
	extern uintptr_t m_bHasHelmet;
	extern uintptr_t m_iObserverMode;
	extern uintptr_t m_bIsScoped;
	extern uintptr_t m_iMoveType;
	extern uintptr_t m_iAccount;
	extern uintptr_t m_iPlayerC4;
	extern uintptr_t dwPlayerResource;
	extern uintptr_t m_iItemDefinitionIndex;

	void InitOffsets();
}