#pragma once

namespace sdk {
	class IMatRenderContext;
	class Ray_t;
	class ITraceFilter;
	class NetChannel;
	class IRecipientFilter;

	struct CViewSetup;
	struct DrawModelState_t;
	struct ModelRenderInfo_t;
	struct trace_t;
	struct bf_write;
}


namespace HOOKS {
	typedef bool( __stdcall* CreateMoveFn )( float, sdk::CUserCmd* );
	typedef void( __thiscall* FrameStageNotifyFn )( void*, int );
	typedef void( __thiscall* PaintTraverseFn )( void*, unsigned int, bool, bool );
	typedef void( __thiscall* DrawModelExecuteFn )( void*, sdk::IMatRenderContext*, const sdk::DrawModelState_t&, const sdk::ModelRenderInfo_t&, matrix3x4_t* );
	typedef void( __thiscall* SceneEndFn )( void* );
	typedef void( __thiscall* SetMouseCodeState_t ) ( void*, ButtonCode_t, MouseCodeState_t );
	typedef void( __thiscall* SetKeyCodeState_t ) ( void*, ButtonCode_t, bool );
	typedef void( __thiscall* TraceRayFn )( void *thisptr, const sdk::Ray_t &ray, unsigned int fMask, sdk::ITraceFilter* pTraceFilter, sdk::trace_t *pTrace );
	typedef void( __thiscall* OverrideViewFn )( void* ecx, sdk::CViewSetup* pSetup );
	typedef int( __thiscall* SendDatagramFn )( sdk::NetChannel*, void* );
	using EmitSound = void( __stdcall* )( sdk::IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, float, int, int, int, const Vector*, const Vector*, void*, bool, float, int );
	typedef float( __thiscall *get_fov_t )( void* );
	//typedef int(__thiscall* ToFirstPersonFn)(void* thisptr);
	typedef bool( __thiscall* SvCheatsGetBoolFn )( void* );
	using PresentFn = long( __stdcall* )( void*, void*, void*, void*, void* );
	using ResetFn = long( __stdcall* )( void*, void* );
	//extern CreateMoveFn original_create_move;
		
	extern PaintTraverseFn original_paint_traverse;

	extern ResetFn original_reset;
	extern PresentFn original_present;
	extern FrameStageNotifyFn original_frame_stage_notify;
	extern DrawModelExecuteFn original_draw_model_execute;
	extern EmitSound oEmitSound;
	extern SceneEndFn original_scene_end;
	extern TraceRayFn original_trace_ray;
	extern SendDatagramFn original_send_datagram;
	extern OverrideViewFn original_override_view;
	//extern ToFirstPersonFn original_to_firstperson;
	extern SvCheatsGetBoolFn original_get_bool;
	extern SetKeyCodeState_t o_SetKeyCodeState;
	extern SetMouseCodeState_t o_SetMouseCodeState;

	extern VMT::VMTHookManager iclient_hook_manager;
	extern VMT::VMTHookManager panel_hook_manager;
	extern VMT::VMTHookManager soundmanager;
	extern VMT::VMTHookManager model_render_hook_manager;
	extern VMT::VMTHookManager render_view_hook_manager;
	extern VMT::VMTHookManager trace_hook_manager;
	extern VMT::VMTHookManager net_channel_hook_manager;
	extern VMT::VMTHookManager override_view_hook_manager;
	extern VMT::VMTHookManager input_table_manager;
	extern VMT::VMTHookManager get_bool_manager;
	extern VMT::VMTHookManager directx_device_manager;
	extern VMT::VMTHookManager input_system_manager;


	void InitHooks( );
	void KillHooks( );

	// virtual function hooks
	bool __stdcall HookedCreateMove( float sample_input_frametime, sdk::CUserCmd* cmd );
	void __fastcall HookedFrameStageNotify( void*, void*, int );
	void __stdcall HookedPaintTraverse( int VGUIPanel, bool ForceRepaint, bool AllowForce );
	void __fastcall HookedDrawModelExecute( void*, void*, sdk::IMatRenderContext*, const sdk::DrawModelState_t&, const sdk::ModelRenderInfo_t&, matrix3x4_t* );
	void __fastcall HookedSceneEnd( void*, void* );
	void __fastcall HookedTraceRay( void *thisptr, void*, const sdk::Ray_t &ray, unsigned int fMask, sdk::ITraceFilter *pTraceFilter, sdk::trace_t *pTrace );
	void __fastcall HookedOverrideView( void* ecx, void* edx, sdk::CViewSetup* pSetup );
	void __fastcall HookedToFirstPerson( void* thisptr, void* );
	int __fastcall hkSendDatagram( sdk::NetChannel* thisptr, void* edx, void* datagram );
	bool __fastcall HookedGetBool( void* pConVar, void* edx );
	long __stdcall hookedpresent( void* device, void* rect_source, void* rect_dest, void* window, void* dregion );
	long __stdcall hookedreset( void* device, void* present_parameters );

	// netvar hooks
	void InitNetvarHooks( );
}