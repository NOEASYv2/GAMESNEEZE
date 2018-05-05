#pragma once
#include <Windows.h>
#include <unordered_map>
#include "../sdk/RecvData.h"

#define k_page_writeable (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)
#define k_page_readable (k_page_writeable|PAGE_READONLY|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_WRITECOPY)
#define k_page_offlimits (PAGE_GUARD|PAGE_NOACCESS)


namespace VMT
{
	class VMTHookManager
	{
	public:
		VMTHookManager() {}

		VMTHookManager(void* instance);
		~VMTHookManager();
		void Init(void* instance);
		void Restore();

		template <class T>
		T HookFunction(int index, void* func)
		{
			/// index is out of range or null function pointer
			if (index < 0 || index >= m_size || !func)
				return reinterpret_cast<T>(nullptr);

			/// get the original (this assumes that you dont try to hook the function twice)
			auto original = reinterpret_cast<T>(m_vtable[index]);
			m_original_funcs[index] = original;

			unsigned long old_protection, tmp;
			VProtect(&m_vtable[index], sizeof(void*), 0x40, &old_protection); /// 0x40 = PAGE_EXECUTE_READWRITE
			m_vtable[index] = func;
			VProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);

			return original;
		}
		void UnHookFunction(int index);

		template <class T>
		T GetOriginalFunction(int index)
		{
			/// index is out of range
			if (index < 0 || index >= m_size)
				return reinterpret_cast<T>(nullptr);

			/// function isn't even hooked - just return the one in the vtable
			if (m_original_funcs.find(index) == m_original_funcs.end())
				return reinterpret_cast<T>(m_vtable[index]);

			return reinterpret_cast<T>(m_original_funcs[index]);
		}

		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}

	private:
		std::unordered_map<int, void*> m_original_funcs;
		void* m_instance = nullptr;
		void** m_vtable = nullptr;
		int m_size = 0;

	private:
		void VProtect(void* address, unsigned int size, unsigned long new_protect, unsigned long* old_protect);
	};
	class NetvarHook {
	private:
		typedef void(*NetvarHookCallback)(const sdk::CRecvProxyData *pData, void *pStruct, void *pOut);
		NetvarHookCallback original_function;
		sdk::RecvProp prop;

	public:
		void Init(sdk::ClientClass* data, std::string table, std::string prop_name, NetvarHookCallback callback) {
			auto client_class = data;

			// go through all classes
			while (client_class) {
				std::string table_name = client_class->m_pRecvTable->m_pNetTableName;

				if (table_name == table) {
					for (int i = 0; i < client_class->m_pRecvTable->m_nProps; i++) {
						auto& prop = client_class->m_pRecvTable->m_pProps[i];
						std::string prop_name = prop.m_pVarName;

						if (prop_name == prop_name) {
							this->prop = client_class->m_pRecvTable->m_pProps[i];
							this->original_function = (NetvarHookCallback)prop.m_ProxyFn;
							prop.m_ProxyFn = callback;
						}
					}
				}

				client_class = client_class->m_pNext;
			}
		}
		void Kill() {
			this->prop.m_ProxyFn = this->original_function;
		}
	};
}