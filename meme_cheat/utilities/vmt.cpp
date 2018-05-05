#include "vmt.h"

namespace VMT
{
	VMTHookManager::VMTHookManager(void* instance)
	{
		Init(instance);
	}
	VMTHookManager::~VMTHookManager()
	{
		Restore();
	}

	void VMTHookManager::Init(void* instance)
	{
		m_instance = instance;
		m_vtable = *static_cast<void***>(instance);

		/// gets the ammount of vfuncs in the vtable
		auto GetSize = [this]() -> int
		{
			int count = 0;
			for (void* curr_func = m_vtable[count]; curr_func != nullptr;)
			{
				MEMORY_BASIC_INFORMATION mbi;
				if (!VirtualQuery(curr_func, &mbi, sizeof(mbi)))
					break;

				/// offlimits
				if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
					break;

				/// not readable
				if (!(mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_WRITECOPY)))
					break;

				count++;
				curr_func = m_vtable[count];
			}

			return count;
		}; m_size = GetSize();
	}
	void VMTHookManager::Restore()
	{
		if (!m_instance || !m_vtable)
			return;

		/// restore all the original functions to the vtable
		for (const auto &func : m_original_funcs)
		{
			unsigned long old_protection, tmp;
			VProtect(&m_vtable[func.first], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protection);
			m_vtable[func.first] = func.second;
			VProtect(&m_vtable[func.first], sizeof(void*), old_protection, &tmp);
		}

		m_original_funcs.clear();
	}
	void VMTHookManager::UnHookFunction(int index)
	{
		/// index is out of range
		if (index < 0 || index >= m_size)
			return;

		/// not even hooked retard
		if (m_original_funcs.find(index) == m_original_funcs.end())
			return;

		auto original = m_original_funcs[index];
		m_original_funcs.erase(index);

		unsigned long old_protection, tmp;
		VProtect(&m_vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protection);
		m_vtable[index] = original;
		VProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);
	}
	void VMTHookManager::VProtect(void* address, unsigned int size, unsigned long new_protect, unsigned long* old_protect)
	{
		VirtualProtect(address, size, new_protect, old_protect);
	}
	/*CVMTHookManager::CVMTHookManager() {}
	CVMTHookManager::~CVMTHookManager()
	{
		this->HookTable(false);
	}
	CVMTHookManager::CVMTHookManager(void* Interface)
	{
		this->Init(Interface);
	}
	bool CVMTHookManager::Init(void* Interface)
	{
		pOrgTable = *(void**)Interface;
		this->count = this->GetCount();
		pCopyTable = malloc(sizeof(void*)* count);
		memcpy(pCopyTable, pOrgTable, sizeof(void*) * count);
		pObject = (DWORD*)Interface;
		return true;
		/*m_ClassBase = Interface;
		m_OldVT = *Interface;
		m_VTSize = GetVTCount(*Interface);
		m_NewVT = new DWORD[m_VTSize + 1];
		memcpy(&m_NewVT[1], m_OldVT, sizeof(DWORD) * m_VTSize);
		m_NewVT[0] = (uintptr_t)m_OldVT[-1];
		DWORD old;
		VirtualProtect(Interface, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old);
		*Interface = &m_NewVT[1];
		VirtualProtect(Interface, sizeof(DWORD), old, &old);
		return true;*/
	/*}
	void CVMTHookManager::HookTable(bool hooked)
	{
		if (hooked)
			*pObject = (DWORD)pCopyTable;
		else
			*pObject = (DWORD)pOrgTable;
	}
	void* CVMTHookManager::HookFunction(int Index, void* hkFunction)
	{
		if (Index < this->count && Index >= 0)
		{
			((DWORD*)pCopyTable)[Index] = (DWORD)hkFunction;
			return (void*)((DWORD*)pOrgTable)[Index];
		}
		return NULL;
	}
	bool CVMTHookManager::CanReadPointer(void* table)
	{
		MEMORY_BASIC_INFORMATION mbi;

		if (table == nullptr)
			return false;
		if (!VirtualQuery(table, &mbi, sizeof(mbi))) 
			return false;
		if (mbi.Protect & k_page_offlimits) 
			return false;

		return mbi.Protect & k_page_readable;
	}
	int CVMTHookManager::GetCount()
	{
		int index = 0; //0
		void** table = ((void**)pOrgTable);
		for (void* fn; (fn = table[index]) != nullptr; index++)
		{
			if (!this->CanReadPointer(fn)) 
				break;
		}
		return index;
	}
	DWORD CVMTHookManager::GetVTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = 0;
		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IS_INTRESOURCE((FARPROC)pdwVMT[dwIndex]))
				break;
			}
		return dwIndex;
	}*/
}