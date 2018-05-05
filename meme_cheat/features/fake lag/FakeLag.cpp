#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/NetChannel.h"
#include "../../sdk/CBaseAnimState.h"
#include "../../sdk/ConVar.h"
#include "../../features/autowall/AutoWall.h"
#include "FakeLag.h"

void CFakeLag::do_fakelag()
{
	GLOBAL::should_send_packet = true;
	int choke_amount;

	auto local_player = ctx::client_ent_list->GetClientEntity(ctx::engine->GetLocalPlayer());
	if (!local_player || local_player->GetHealth() <= 0)
		return;

	auto net_channel = ctx::engine->GetNetChannel();
	if (!net_channel)
		return;

	if (Vars::options.aa_type == 0 || Vars::options.aa_type == 5)
		return;

	//if u dont remember, 1 = no fakelag so right now there is no fakelag (fucks with shit, nice fakelag bolbi1!1!!!!)
	if (Vars::options.lag_bool)
	{
		//if (fake_walk)
			//choke_amount = 14;
		//else
		//{
			choke_amount = 1; // standing flag (1)
			if (!(local_player->GetFlags() & FL_ONGROUND))
			{
				choke_amount = Vars::options.jump_lag; // jumping flag (6)
			}
			else if (local_player->GetVelocity().Length2D() > 0.1)
			{
				choke_amount = Vars::options.move_lag; // moving flag (3)
			}
		//}
	}
	else
		choke_amount = 1;

	if (net_channel->m_nChokedPackets >= min(15, choke_amount))
		GLOBAL::should_send_packet = true;
	else
		GLOBAL::should_send_packet = false;
}

CFakeLag* fakelag = new CFakeLag();