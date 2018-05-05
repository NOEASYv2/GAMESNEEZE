#pragma once

#include "../../sdk/GameEvents.h"
#include "../../sdk/CBaseEntity.h"

#define CREATE_EVENT_LISTENER(class_name)\
class class_name : public sdk::IGameEventListener\
{\
public:\
	~class_name() { ctx::game_event_manager->RemoveListener(this); }\
\
	virtual void FireGameEvent(sdk::IGameEvent* game_event);\
};\

namespace CGameEvents //fuck namespaces, fuck ur style bolbi
{
	// put this in your header
	void InitializeEventListeners( );

	void RemoveListeners( );

	CREATE_EVENT_LISTENER( PlayerHurtListener );
	CREATE_EVENT_LISTENER( PlayerDeathListener );
	CREATE_EVENT_LISTENER( BulletImpactListener );
	CREATE_EVENT_LISTENER( ItemPurchaseListener );
};