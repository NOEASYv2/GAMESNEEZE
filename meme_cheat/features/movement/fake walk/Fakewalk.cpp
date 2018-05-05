#include "../../../includes.h"
#include "../../../utilities/interfaces.h"
#include "../../../sdk/IEngine.h"
#include "../../../sdk/CUserCmd.h"
#include "../../../sdk/CBaseEntity.h"
#include "../../../sdk/CClientEntityList.h"
#include "../../../sdk/CTrace.h"
#include "../../../sdk/CBaseWeapon.h"
#include "../../../sdk/CGlobalVars.h"
#include "../../../sdk/NetChannel.h"
#include "../../../sdk/CBaseAnimState.h"
#include "../../../sdk/ConVar.h"
#include "../../features/autowall/autowall.h"
#include "fakewalk.h"
#include "../../features/aimbot/Aimbot.h"

#include <time.h>
#include <iostream>

void CFakewalk::do_fakewalk(sdk::CUserCmd* pCmd, bool & bSendPacket )
{
	auto local = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	if ( ctx::input_system->IsButtonDown( ( ButtonCode_t ) Vars::options.fakewalk ) ) {
		static int choked = 0;
		choked = choked > 7 ? 0 : choked + 1;
		pCmd->forwardmove = choked < 2 || choked > 5 ? 0 : pCmd->forwardmove;
		pCmd->sidemove = choked < 2 || choked > 5 ? 0 : pCmd->sidemove;
		bSendPacket = choked < 1;
	}
}

CFakewalk* slidebitch = new CFakewalk();