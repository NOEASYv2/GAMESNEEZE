#pragma once
namespace sdk
{
	class CUserCmd;
	class CBaseEntity;
}

class CAntiAim
{
public:
	bool GetBestHeadAngle( Vector & angle );
	void antiaimyaw( sdk::CUserCmd * cmd );
	void do_antiaim(sdk::CUserCmd* cmd);
	void fix_movement(sdk::CUserCmd* cmd);
	void freestanding( sdk::CUserCmd * cmd, sdk::CBaseEntity * local );
	void backwardsf( sdk::CUserCmd * cmd );
	void sidewaysf( sdk::CUserCmd * cmd );
	void backjitterf( sdk::CUserCmd * cmd );
	void lowerbodyf( sdk::CUserCmd * cmd );
private:
	void backwards(sdk::CUserCmd* cmd);
	void legit(sdk::CUserCmd* cmd);
	void sideways(sdk::CUserCmd* cmd);
	void backjitter(sdk::CUserCmd* cmd);
};

extern CAntiAim* antiaim;