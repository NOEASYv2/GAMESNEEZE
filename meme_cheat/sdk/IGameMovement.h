#pragma once
#include "../utilities/vector3D.h"

namespace sdk {
	class CBaseEntity;
	class CMoveData;

	class IGameMovement {
	public:
		virtual			~IGameMovement( void ) { }

		virtual void	ProcessMovement( sdk::CBaseEntity *pPlayer, sdk::CMoveData *pMove ) = 0;
		virtual void	Reset( void ) = 0;
		virtual void	StartTrackPredictionErrors( sdk::CBaseEntity *pPlayer ) = 0;
		virtual void	FinishTrackPredictionErrors( sdk::CBaseEntity *pPlayer ) = 0;
		virtual void	DiffPrint( char const *fmt, ... ) = 0;

		virtual Vector const&	GetPlayerMins( bool ducked ) const = 0;
		virtual Vector const&	GetPlayerMaxs( bool ducked ) const = 0;
		virtual Vector const&   GetPlayerViewOffset( bool ducked ) const = 0;

		virtual bool			IsMovingPlayerStuck( void ) const = 0;
		virtual sdk::CBaseEntity*	GetMovingPlayer( void ) const = 0;
		virtual void			UnblockPusher( sdk::CBaseEntity* pPlayer, sdk::CBaseEntity *pPusher ) = 0;

		virtual void SetupMovementBounds( sdk::CMoveData *pMove ) = 0;
	};
}