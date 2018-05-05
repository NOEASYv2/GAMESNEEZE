#pragma once

#include "../utilities/vmt.h"
#include "CBaseEntity.h"

namespace sdk
{
	class CBaseEntity;

	class CClientEntityList
	{
	public:

		CBaseEntity* GetClientEntity(int index)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, int);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 3)(this, index);
		}

		int GetHighestEntityIndex()
		{
			typedef int(__thiscall* Fn)(void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 8)(this);
		}

		CBaseEntity* GetClientEntityFromHandle(void* handle)
		{
			typedef CBaseEntity*(__thiscall* Fn)(void*, void*);
			return VMT::VMTHookManager::GetFunction<Fn>(this, 7)(this, handle);
		}
	};
}