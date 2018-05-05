#include "../includes.h"
#include "../sdk/CClientState.h"

#include "interfaces.h"


namespace ctx
{
	 sdk::IClient* client;
	 sdk::CInput* input;
	 sdk::CPanel* panel;
	 sdk::IEngineSound* sound;
	 sdk::CClientState** client_state;
	 sdk::ISurface* surface;
	 sdk::IEngine* engine;
	 sdk::CClientEntityList* client_ent_list;
	 sdk::CGlobalVars* globals;
	 sdk::IVDebugOverlay* debug_overlay;
	 sdk::CTrace* trace;
	 sdk::CModelInfo* model_info;
	 sdk::ICVar* convar;
	 sdk::CInputSystem* input_system;
	 sdk::CModelRender* model_render;
	 sdk::CRenderView* render_view;
	 sdk::IMaterialSystem* material_system;
	 sdk::IPhysicsProps* physics_props;
	 sdk::COverrideView* client_mode;
	 sdk::IGameEventManager* game_event_manager;
	 sdk::IViewRenderBeams* view_render_beams;
	 sdk::IMoveHelper* move_helper;
	 sdk::IGameMovement* game_movement;
	 sdk::IPrediction* prediction;

	 DWORD *input_internal;

	 typedef void* ( *CreateInterfaceFn )( const char *pName, int *pReturnCode );
	 CreateInterfaceFn GetModuleFactory( HMODULE module ) {
		 return reinterpret_cast<CreateInterfaceFn>( GetProcAddress( module, "CreateInterface" ) );
	 }
	 template<typename T>
	 T* GetInterface( CreateInterfaceFn f, const char* szInterfaceVersion ) {
		 auto result = reinterpret_cast<T*>( f( szInterfaceVersion, nullptr ) );

		 return result;
	 }

	void InitInterfaces()
	{
		auto ClientPointer = get_module_factory( GetModuleHandleW( L"client.dll" ) );
		//client_state = *( sdk::CClientState*** ) ( UTILS::FindSignature( "engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3" ) + 1 );
		client = reinterpret_cast<sdk::IClient*>(FindInterface("client.dll", "VClient"));
		input = *reinterpret_cast<sdk::CInput**>((*(DWORD**) client )[15] + 0x1);
		panel = reinterpret_cast<sdk::CPanel*>(FindInterface("vgui2.dll", "VGUI_Panel")); 
	//	sound = reinterpret_cast<sdk::IEngineSound*>( FindInterface( "engine.dll", "IEngineSoundClient003" ) );
		sound = GetInterface<sdk::IEngineSound>( GetModuleFactory( GetModuleHandleW( L"engine.dll" ) ), "IEngineSoundClient003" );
		surface = reinterpret_cast<sdk::ISurface*>(FindInterface("vguimatsurface", "VGUI_Surface"));
		engine = reinterpret_cast<sdk::IEngine*>(FindInterface("engine.dll", "VEngineClient"));
		client_state = *( sdk::CClientState*** ) ( ( *( DWORD** ) engine ) [ 12 ] + 0x10 );
		client_ent_list = reinterpret_cast<sdk::CClientEntityList*>(FindInterface("client.dll", "VClientEntityList"));
		debug_overlay = reinterpret_cast<sdk::IVDebugOverlay*>(FindInterface("engine.dll", "VDebugOverlay"));
		trace = reinterpret_cast<sdk::CTrace*>(FindInterface("engine.dll", "EngineTraceClient"));
		model_info = reinterpret_cast<sdk::CModelInfo*>(FindInterface("engine.dll", "VModelInfoClient"));
		convar = reinterpret_cast<sdk::ICVar*>(FindInterface("vstdlib.dll", "VEngineCvar"));
		input_system = reinterpret_cast<sdk::CInputSystem*>(FindInterface("inputsystem.dll", "InputSystemVersion"));
		input_internal = reinterpret_cast<DWORD*>( FindInterface( "vgui2.dll", "VGUI_InputInternal" ) );
		model_render = reinterpret_cast<sdk::CModelRender*>(FindInterface("engine.dll", "VEngineModel")); 
		render_view = reinterpret_cast<sdk::CRenderView*>(FindInterface("engine.dll", "VEngineRenderView"));
		material_system = reinterpret_cast<sdk::IMaterialSystem*>(FindInterface("materialsystem.dll", "VMaterialSystem"));
		physics_props = reinterpret_cast<sdk::IPhysicsProps*>(FindInterface("vphysics.dll", "VPhysicsSurfaceProps"));
		game_event_manager = reinterpret_cast<sdk::IGameEventManager*>(FindInterfaceEx("engine.dll", "GAMEEVENTSMANAGER002"));
		view_render_beams = *reinterpret_cast<sdk::IViewRenderBeams**>(UTILS::FindSignature("client.dll", "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08") + 0x1);
		game_movement = ( sdk::IGameMovement* ) ClientPointer( "GameMovement001", nullptr );
		move_helper = **( sdk::IMoveHelper*** ) ( UTILS::FindPattern( "client.dll", ( PBYTE )"\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68", "xx????xxxx" ) + 2 );
		prediction = ( sdk::IPrediction* ) ClientPointer( "VClientPrediction001", nullptr );


		globals = **reinterpret_cast<sdk::CGlobalVars***>((*reinterpret_cast< DWORD** >( client ))[0] + 0x1B);
		void** pClientTable = *reinterpret_cast<void***>( client );
		client_mode = **reinterpret_cast<sdk::COverrideView***>(reinterpret_cast<DWORD>(pClientTable[10]) + 5);
	}

	void* FindInterface(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
			GetModuleHandleA(Module), "CreateInterface"));

		char PossibleInterfaceName[1024];
		for (int i = 1; i < 100; i++)
		{
			sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;

			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;
		}

		if (!Interface)
			std::cout << InterfaceName << " not found!" << std::endl;
		else
			std::cout << InterfaceName << " 0x" << Interface << std::endl;

		return Interface;
	}
	void* FindInterfaceEx(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
			GetModuleHandleA(Module), "CreateInterface"));

		Interface = CreateInterface(InterfaceName, 0);
		if (!Interface)
			std::cout << InterfaceName << " not found!" << std::endl;

		return Interface;
	}
}