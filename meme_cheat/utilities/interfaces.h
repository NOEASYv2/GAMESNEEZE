#pragma once

namespace sdk
{
	class IClient;
	class CInput;
	class CPanel;
	class ISurface;
	class IEngine;
	class CClientEntityList;
	class CGlobalVars;
	class IVDebugOverlay;
	class CTrace;
	class CModelInfo;
	class ICVar;
	class CInputSystem;
	class CModelRender;
	class CRenderView;
	class IMaterialSystem;
	class CGlowObjectDefinition;
	class CGlowObjectManager;
	class IPhysicsProps;
	class COverrideView;
	class IGameEventManager;
	class IViewRenderBeams;
	class CClientState;
	class IMoveHelper;
	class IPrediction;
	class IGameMovement;
	class IEngineSound;
}

namespace ctx
{
	extern sdk::IClient* client;
	extern sdk::CInput* input;
	extern sdk::CPanel* panel; 
	extern sdk::IEngineSound* sound;
	extern sdk::CClientState** client_state;
	extern sdk::ISurface* surface;
	extern sdk::IEngine* engine;
	extern sdk::CClientEntityList* client_ent_list;
	extern sdk::CGlobalVars* globals;
	extern sdk::IVDebugOverlay* debug_overlay;
	extern sdk::CTrace* trace;
	extern sdk::CModelInfo* model_info;
	extern sdk::ICVar* convar;
	extern sdk::CInputSystem* input_system;
	extern sdk::CModelRender* model_render;
	extern sdk::CRenderView* render_view;
	extern sdk::IMaterialSystem* material_system;
	extern sdk::IPhysicsProps* physics_props;
	extern sdk::COverrideView* client_mode;
	extern sdk::IGameEventManager* game_event_manager;
	extern sdk::IViewRenderBeams* view_render_beams;
	extern sdk::IMoveHelper* move_helper;
	extern sdk::IGameMovement* game_movement;
	extern sdk::IPrediction* prediction;

	extern DWORD *input_internal;

	void InitInterfaces();

	typedef void* (*CreateInterfaceFn)(const char *Name, int *ReturnCode);
	void* FindInterface(const char* Module, const char* InterfaceName);
	void* FindInterfaceEx(const char* Module, const char* InterfaceName);
	template<class T>
	static T* FindHudElement( const char* name ) {
		static auto pThis = *reinterpret_cast<DWORD**>( U::pattern_scan( GetModuleHandleW( L"client.dll" ), "B9 ? ? ? ? E8 ? ? ? ? 83 7D F8 ?" ) + 1 );

		static auto find_hud_element = reinterpret_cast<DWORD( __thiscall* )( void*, const char* )>( U::pattern_scan( GetModuleHandleW( L"client.dll" ), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) );
		return ( T* ) find_hud_element( pThis, name );
	}
	typedef void* ( __cdecl* CreateInterface_t )( const char*, int* );
	typedef void* ( *CreateInterfaceFn )( const char* pName, int* pReturnCode );
	inline CreateInterfaceFn get_module_factory( HMODULE module ) {
		return reinterpret_cast<CreateInterfaceFn>( GetProcAddress( module, "CreateInterface" ) );
	}
}