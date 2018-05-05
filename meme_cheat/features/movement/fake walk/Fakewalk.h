#pragma once

namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
}

class CFakewalk
{
public:
	void do_fakewalk( sdk::CUserCmd * pCmd );
	void do_fakewalk( sdk::CUserCmd * pCmd, bool & bSendPacket );
};

extern CFakewalk* slidebitch;