#pragma once

namespace sdk
{
	class CRecvProxyData;
}
namespace UTILS
{
	typedef void(*NetvarHookCallback)(const sdk::CRecvProxyData *pData, void *pStruct, void *pOut);

	class CNetvarHookManager
	{
	public:
		void Hook(std::string table, std::string prop, NetvarHookCallback callback);

	private:
	}; extern CNetvarHookManager netvar_hook_manager;
}