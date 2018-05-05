#include "../../includes.h"

#include "interfaces.h"
#include "../sdk/IClient.h"
#include "../sdk/RecvData.h"

#include "NetvarHookManager.h"

namespace UTILS
{
	CNetvarHookManager netvar_hook_manager;
	void CNetvarHookManager::Hook(std::string table_to_hook, std::string prop_to_hook, NetvarHookCallback callback)
	{
		auto client_class = ctx::client->GetAllClasses();

		// go through all classes
		while (client_class)
		{
			std::string table_name = client_class->m_pRecvTable->m_pNetTableName;

			if (table_name == table_to_hook)
			{
				for (int i = 0; i < client_class->m_pRecvTable->m_nProps; i++)
				{
					auto& prop = client_class->m_pRecvTable->m_pProps[i];
					std::string prop_name = prop.m_pVarName;

					if (prop_name == prop_to_hook)
					{
						prop.m_ProxyFn = callback;
					}
				}
			}

			client_class = client_class->m_pNext; // go to the next class
		}	
	}
}