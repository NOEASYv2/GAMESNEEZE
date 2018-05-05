#pragma once

#include "../utilities/vmt.h"

namespace sdk
{
	class CPanel
	{
	public:
		const char* GetName(unsigned int Panel)
		{
			typedef const char*(__thiscall* Fn)(void*, unsigned int);
			return (VMT::VMTHookManager::GetFunction<Fn>(this, 36))(this, Panel);
		}
		void SetMouseInputEnabled(unsigned int iPanel, bool bState)
		{
			return (VMT::VMTHookManager::GetFunction<void(__thiscall *)(PVOID, int, bool)>(this, 32))(this, iPanel, bState);
		}
	};
}