#pragma once

namespace sdk
{
	class CUserCmd;
}

class CMovement
{
public:
	void bunnyhop(sdk::CUserCmd* cmd);
	void set_ct( const char * tag, const char * name );
	void set_ct( const char * tag );
	void autostrafer(sdk::CUserCmd* cmd);
	void KnifeBot( sdk::CUserCmd * pCmd );
};

extern CMovement* movement;