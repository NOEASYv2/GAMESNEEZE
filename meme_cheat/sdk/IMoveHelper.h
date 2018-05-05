#pragma once

namespace sdk {
	class CBaseEntity;
	class IMoveHelper {
	private:
		virtual void UnknownVirtual( ) = 0;
	public:
		virtual void SetHost( sdk::CBaseEntity* host ) = 0;
	};
}