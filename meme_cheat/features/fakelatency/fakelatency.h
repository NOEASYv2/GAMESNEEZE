#pragma once
#include "../../includes.h"

#include "../../hooks/hooks.h"
#include "../../utilities/interfaces.h"
#include "../../utilities/offsets.h"
#include "../../utilities/Vector4D.h"
#include "../../utilities/NetvarHookManager.h"
#include "../../utilities/render.h"

#include "../../sdk/CInput.h"
#include "../../sdk/CClientState.h"
#include "../../sdk/IClient.h"
#include "../../sdk/CPanel.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/CGlowObjectManager.h"
#include "../../sdk/IVDebugOverlay.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CBaseAnimState.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/RecvData.h"
#include "../../sdk/ModelInfo.h"
#include "../../sdk/ModelRender.h"
#include "../../sdk/RenderView.h"
#include "../../sdk/NetChannel.h"
#include "../../sdk/CIncomingSequences.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CViewSetup.h"
#include "../../sdk/CGlobalVars.h"

#include "../../features/movement/Movement.h"
#include "../../features/visual/Visuals.h"
#include "../../features/visual/models/Chams.h"
#include "../../features/anti-aim/AntiAim.h"
#include "../../features/aimbot/Aimbot.h"
#include "../../features/engine prediction/EnginePrediction.h"
#include "../../features/resolver/Resolver.h"
#include "../../features/lag compensation/Backtracking.h"
#include "../../features/movement/fake walk/FakeWalk.h"
#include "../../features/fake lag/FakeLag.h"
#include "../../sdk/GameEvents.h"
#include "../../gui/textures.h"
#include "../../gui/menu_framework.h"
#include "../../gui/newgui/menu.hpp"

class CIncomingSequence {
public:
	int in_reliable_state,
		out_reliable_state,
		in_sequence_nr;
	float realtime;

	CIncomingSequence( ) {
		in_reliable_state = ( *ctx::client_state )->m_NetChannel->m_nInReliableState;
		out_reliable_state = ( *ctx::client_state )->m_NetChannel->m_nOutReliableState;
		in_sequence_nr = ( *ctx::client_state )->m_NetChannel->m_nInSequenceNr;
		realtime = ctx::globals->realtime;
	}

	CIncomingSequence( int irs, int ors, int isn, float _realtime ) {
		in_reliable_state = irs;
		out_reliable_state = ors;
		in_sequence_nr = isn;
		realtime = _realtime;
	}
};

class CFakeLatency {
public:
	int SendDatagram( sdk::NetChannel* netchan, void* datagram );
	void UpdateIncomingSequences( ClientFrameStage_t curStage );
	void AddLatency( sdk::NetChannel* netchan, float latency );

	std::deque <CIncomingSequence> m_sequences;
	float lastincomingsequencenumber;

	sdk::NetChannel* m_netchan;
	int ticks = 0;
};

extern CFakeLatency* g_FakeLatency;