#pragma once
#include "../../includes.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/CMoveData.h"
#include "../../sdk/IMoveHelper.h"
#include "../../sdk/IGameMovement.h"
#include "../../sdk/IPrediction.h"
#include "../../sdk/CUserCMD.h"
#include "../../utilities/variables.h"
#include "../../utilities/Vector4D.h"
#include "../../sdk/CInput.h"
#include "../../sdk/IClient.h"
#include "../../sdk/CPanel.h"
#include "../../utilities/render.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/CGlowObjectManager.h"
#include "../../sdk/IVDebugOverlay.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CBaseAnimState.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/RecvData.h"
#include "../../utilities/NetvarHookManager.h"
#include "../../sdk/ModelInfo.h"
#include "../../sdk/ModelRender.h"
#include "../../sdk/RenderView.h"
#include "../../sdk/NetChannel.h"
#include "../../sdk/CIncomingSequences.h"
#include "../../sdk/CTrace.h"
#include "../../sdk/CViewSetup.h"
#include "../../sdk/CGlobalVars.h"
#include "../../features/movement/movement.h"
#include "../../features/visual/Visuals.h"
#include "../../features/visual/models/Chams.h"
#include "../../features/anti-aim/AntiAim.h"
#include "../../features/aimbot/Aimbot.h"
#include "../../features/resolver/Resolver.h"
#include "../../features/lag compensation/Backtracking.h"
#include "../../features/movement/fake walk/Fakewalk.h"
#include "../../features/fake lag/FakeLag.h"
#include "../../sdk/GameEvents.h"
#include "../../gui/menu_framework.h"

class CPredictionSystem {
public:
	void StartPrediction( sdk::CUserCmd * pCmd );
	void EndPrediction( sdk::CUserCmd * pCmd );
private:
	float m_flOldCurtime;
	float m_flOldFrametime;
	sdk::CMoveData m_MoveData;
	int* m_nPredictionRandomSeed;
	int* m_pPredictionRandomSeed;
	
};

extern CPredictionSystem* prediction_system;