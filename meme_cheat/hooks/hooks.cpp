#include "..\includes.h"

#include "hooks.h"
#include "../utilities/interfaces.h"
#include "../utilities/offsets.h"
#include "../utilities/Vector4D.h"
#include "../utilities/NetvarHookManager.h"
#include "../utilities/render.h"

#include "../sdk/CInput.h"
#include "../sdk/CClientState.h"
#include "../sdk/IClient.h"
#include "../sdk/CPanel.h"
#include "../sdk/ConVar.h"
#include "../sdk/CGlowObjectManager.h"
#include "../sdk/IVDebugOverlay.h"
#include "../sdk/IEngine.h"
#include "../sdk/CBaseWeapon.h"
#include "../sdk/CBaseAnimState.h"
#include "../sdk/CTrace.h"
#include "../sdk/CClientEntityList.h"
#include "../sdk/RecvData.h"
#include "../sdk/ModelInfo.h"
#include "../sdk/ModelRender.h"
#include "../sdk/RenderView.h"
#include "../sdk/NetChannel.h"
#include "../sdk/CIncomingSequences.h"
#include "../sdk/CTrace.h"
#include "../sdk/CViewSetup.h"
#include "../sdk/CGlobalVars.h"

#include "../features/movement/Movement.h"
#include "../features/visual/Visuals.h"
#include "../features/visual/models/Chams.h"
#include "../features/fakelatency/fakelatency.h"
#include "../features/anti-aim/AntiAim.h"
#include "../features/aimbot/Aimbot.h"
#include "../features/engine prediction/EnginePrediction.h"
#include "../features/resolver/Resolver.h"
#include "../features/lag compensation/Backtracking.h"
#include "../features/movement/fake walk/FakeWalk.h"
#include "../features/fake lag/FakeLag.h"
#include "../sdk/GameEvents.h"
#include "../gui/textures.h"
#include "../gui/menu_framework.h"
#include "../gui/newgui/menu.hpp"

#include <intrin.h>

Vector fakeangles;

//--- Other Globally Used Variables ---///
static bool tick = false;

//--- Declare Signatures and Patterns Here ---///
static auto CAM_THINK = UTILS::FindSignature( "client.dll", "85 C0 75 30 38 86" );
//sdk::CGlowObjectManager* pGlowObjectManager = (sdk::CGlowObjectManager*)(UTILS::FindSignature("client.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 0x3);
static auto linegoesthrusmoke = UTILS::FindPattern( "client.dll", ( PBYTE )"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx" );

namespace HOOKS {
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	EmitSound oEmitSound;
	FrameStageNotifyFn original_frame_stage_notify;
	ResetFn original_reset;
	PresentFn original_present;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	SendDatagramFn original_send_datagram = 0;
	OverrideViewFn original_override_view;
	SvCheatsGetBoolFn original_get_bool;
	SetMouseCodeState_t o_SetMouseCodeState;
	SetKeyCodeState_t o_SetKeyCodeState;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager soundmanager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager override_view_hook_manager;
	VMT::VMTHookManager input_table_manager;
	VMT::VMTHookManager prediction_bool_manager;
	VMT::VMTHookManager get_bool_manager;
	VMT::VMTHookManager directx_device_manager;
	VMT::VMTHookManager input_system_manager;


	//	texture* zdj;

	get_fov_t o_vfov;
	void AutoRevolver( sdk::CUserCmd* cmd ) {
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		auto weapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) ) );

		if ( !weapon )
			return;

		if ( local_player->GetHealth( ) <= 0 )
			return;

		if ( !weapon->GetItemDefenitionIndex( ) )
			return;

		if ( weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_REVOLVER ) {
			static int delay = 0;
			delay++;
			if ( delay <= 15 )
				cmd->buttons |= IN_ATTACK;
			else
				delay = 0;
		}
	}
	void animate( std::string& clantag ) {
		std::string temp = clantag;
		clantag.erase( 0, 1 );
		clantag += temp [ 0 ];
	}
	void __stdcall hkdEmitSound( sdk::IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity ) { }
	void __fastcall SetKeyCodeState_h( void* thisptr, void* EDX, ButtonCode_t code, bool bDown ) {
		if ( GLOBAL::input_shouldListen && bDown ) {
			GLOBAL::input_shouldListen = false;
			if ( GLOBAL::input_receivedKeyval )
				*GLOBAL::input_receivedKeyval = code;
		}
		return o_SetKeyCodeState( thisptr, code, bDown );
	}

	void __fastcall SetMouseCodeState_h( void* thisptr, void* EDX, ButtonCode_t code, MouseCodeState_t state ) {
		if ( GLOBAL::input_shouldListen && state == BUTTON_PRESSED ) {
			GLOBAL::input_shouldListen = false;
			if ( GLOBAL::input_receivedKeyval )
				*GLOBAL::input_receivedKeyval = code;
		}

		return o_SetMouseCodeState( thisptr, code, state );
	}
	void AutoZeus( sdk::CUserCmd* cmd ) {
		auto localplayer = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		auto weapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( localplayer->GetActiveWeaponIndex( ) ) );
		Vector traceStart, traceEnd;
		sdk::trace_t tr;

		Vector viewAngles;
		ctx::engine->GetViewAngles( viewAngles );
		Vector viewAnglesRcs = viewAngles + *localplayer->AimPunchAngle( ) * 2.0f;

		MATH::AngleVectors( viewAnglesRcs, &traceEnd );

		traceStart = localplayer->GetEyeAngles( );
		traceEnd = traceStart + ( traceEnd * 8192.0f );

		sdk::Ray_t ray;
		ray.Init( traceStart, traceEnd );
		sdk::CTraceFilter traceFilter;
		traceFilter.pSkip1 = localplayer;
		ctx::trace->TraceRay( ray, 0x46004003, &traceFilter, &tr );

		sdk::CBaseEntity * player = ( sdk::CBaseEntity* ) tr.m_pEnt;
		if ( !player )
			return;

		if ( player == localplayer
			|| player->GetIsDormant( )
			|| !player->GetHealth( ) > 0
			|| player->GetImmunity( ) )
			return;

		//	if ( player->GetTeam( ) == localplayer->GetTeam( ) )
		//		return;

		float playerDistance = localplayer->GetAbsOrigin( ).DistTo( player->GetAbsOrigin( ) );

		if ( weapon->GetItemDefenitionIndex( ) != sdk::ItemDefinitionIndex::WEAPON_TASER )
			return;

		if ( !weapon )
			return;

		if ( !player )
			return;

		if ( !localplayer )
			return;

		if ( player == localplayer
			|| player->GetIsDormant( )
			|| !player->GetHealth( ) > 0
			|| player->GetImmunity( ) )
			return;

		if ( weapon->GetItemDefenitionIndex( ) == sdk::ItemDefinitionIndex::WEAPON_TASER ) {
			if ( playerDistance <= 184.f )
				cmd->buttons |= IN_ATTACK;
		}
	}
	bool __stdcall HookedCreateMove( float sample_input_frametime, sdk::CUserCmd* cmd ) {
		if ( !cmd || cmd->command_number == 0 )
			return false;

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		bool* SendPacket = ( bool* ) ( *FPointer - 0x1C );

		if ( !SendPacket )
			return false;

		GLOBAL::should_send_packet = *SendPacket;
		// S T A R T

		if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) )
			if ( Vars::options.bhop_bool ) movement->bunnyhop( cmd );

		prediction_system->StartPrediction( cmd );
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		auto weapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) ) );

		if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
			if ( Vars::options.aim_type == 0 )
				slidebitch->do_fakewalk( cmd, *SendPacket );

			if ( Vars::options.strafe_bool )
				movement->autostrafer( cmd );



			static auto NewSignonMsg = reinterpret_cast< void( __thiscall* )( void*, int, int ) >( UTILS::pattern_scan( GetModuleHandleA( "engine.dll" ), "55 8B EC 56 57 8B F9 8D 4F 04 C7 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B ?? ?? C6 ?? ?? ?? C7" ) );
			static auto ClearSignonMsg = reinterpret_cast< void( __thiscall* )( void* ) >( UTILS::pattern_scan( GetModuleHandleA( "engine.dll" ), "53 56 57 8B F9 8D ?? 38 C7 ?? ?? ?? ?? ?? C7 ?? ?? ?? ?? ?? ?? 8B" ) );

			void* pSignonMsg = malloc( 76 );

			int nTotalSentSignon = 0;

			if ( Vars::options.aim_type == 0 ) {
				aimbot->shoot_enemy( cmd );
				aimbot->fix_recoil( cmd );
				backtracking->backtrack_player( cmd );
				fakelag->do_fakelag( );
			}

			if ( Vars::options.aim_type == 1 && Vars::options.back_bool )
				backtracking->run_legit( cmd );

			if ( Vars::options.aa_bool || Vars::options.aa_type > 0 || Vars::options.aim_type == 0 ) {
				antiaim->do_antiaim( cmd );
				antiaim->fix_movement( cmd );
			}

			if ( Vars::options.clantag ) {
				static float iLastTime;

				if ( int( ctx::globals->curtime ) != iLastTime ) {
					switch ( int( ctx::globals->curtime ) % 11 ) {
						case 0: movement->set_ct( ( "gamesneeze" ) ); break;
						case 1: movement->set_ct( ( "amesneeze g" ) ); break;
						case 2: movement->set_ct( ( "mesneeze ga" ) ); break;
						case 3: movement->set_ct( ( "esneeze gam" ) ); break;
						case 4: movement->set_ct( ( "sneeze game" ) ); break;
						case 5: movement->set_ct( ( "neeze games" ) ); break;
						case 6: movement->set_ct( ( "eeze gamesn" ) ); break;
						case 7: movement->set_ct( ( "eze gamesne" ) ); break;
						case 8: movement->set_ct( ( "ze gamesnee" ) ); break;
						case 9: movement->set_ct( ( "e gamesneez" ) ); break;
						case 10: movement->set_ct( ( " gamesneeze" ) ); break;
					}
				}

				iLastTime = int( ctx::globals->curtime );

			}
			else {
				movement->set_ct( "" );
			}

			if ( Vars::options.auto_Revolver )
				AutoRevolver( cmd );

			if ( !GLOBAL::should_send_packet )
				GLOBAL::real_angles = cmd->viewangles;


			if ( GLOBAL::should_send_packet )
				fakeangles = cmd->viewangles;

			if ( !GLOBAL::should_send_packet )
				GLOBAL::nChockedTicks++;
			else
				GLOBAL::nChockedTicks = 0;
		}

		// E N D 
		*SendPacket = GLOBAL::should_send_packet;

		prediction_system->EndPrediction( cmd );
		UTILS::ClampLemon( cmd->viewangles );

		return false;
	}
	void __stdcall HookedPaintTraverse( int VGUIPanel, bool ForceRepaint, bool AllowForce ) {


		std::string panel_name = ctx::panel->GetName( VGUIPanel );

		if ( panel_name == "HudZoom" )
			return;

		if ( panel_name == "MatSystemTopPanel" ) {
			if ( FONTS::ShouldReloadFonts( ) )
				FONTS::InitFonts( );

			if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
				if ( Vars::options.esp_bool ) {
					visuals->Draw( );
					visuals->ClientDraw( );
				}
			}

			MENU::PPGUI_PP_GUI::Begin( );
			MENU::Do( );
			MENU::PPGUI_PP_GUI::End( );

			//	zdj->draw( 10, 10, zdj->wi, zdj->he );

			UTILS::INPUT::input_handler.Update( );

			RENDER::DrawSomething( );
		}

		original_paint_traverse( ctx::panel, VGUIPanel, ForceRepaint, AllowForce );
	}
	void OverrideYaw( ) {
		if ( ctx::input_system->IsButtonDown( ( ButtonCode_t ) Vars::options.override_key ) ) {
			int w, h, x, y;
			ctx::engine->GetScreenSize( w, h );
			x = w / 2;
			y = h / 2;
			Vector crosshair = Vector( x, y, 0 );
			int bestent = -1;
			float best_fov = 0;
			Vector besthead2d;
			for ( int i = 0; i <= ctx::globals->maxclients; i++ ) {
				sdk::CBaseEntity* player = ctx::client_ent_list->GetClientEntity( i );

				if ( !player->GetHealth( ) > 0 )
					continue;
				if ( player->GetIsDormant( ) )
					continue;
				if ( !player )
					continue;

				Vector headpos3d = player->GetBonePosition( 8 ), headpos2d;
				if ( !RENDER::WorldToScreen( headpos3d, headpos2d ) )
					continue;
				float fov2d = crosshair.DistTo( headpos2d );
				if ( bestent == -1 || fov2d < best_fov ) {
					bestent = 1;
					best_fov = fov2d;
					besthead2d = headpos2d;
				}
			}
			if ( bestent > -1 ) {
				sdk::CBaseEntity* player = ctx::client_ent_list->GetClientEntity( bestent );
				int minx = x - ( w / 10 ), maxx = x + ( w / 10 );
				if ( besthead2d.x < x || besthead2d.x > x )
					return;

				int totalwidth = maxx - minx;
				int playerx = besthead2d.x - minx;
				int additiveyaw = -( ( ( playerx * 360 ) / totalwidth ) - 100 );

				Vector* hr = player->GetHeadRotation( );

				hr->y += additiveyaw;
			}
		}

	}
	void __fastcall HookedFrameStageNotify( void* ecx, void* edx, int stage ) {
		Vector vecAimPunch;
		Vector vecViewPunch;

		Vector* pAimPunch = nullptr;
		Vector* pViewPunch = nullptr;

		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		switch ( stage ) {
			case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
				//--- Angle Resolving and Such ---//
				if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
					for ( int i = 1; i < 65; i++ ) {
						auto entity = ctx::client_ent_list->GetClientEntity( i );

						if ( !entity )
							continue;

						if ( !local_player )
							continue;

						bool is_local_player = entity == local_player;
						bool is_teammate = local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player;

						if ( is_local_player )
							continue;

						if ( is_teammate )
							continue;

						if ( entity->GetHealth( ) <= 0 )
							continue;

						//backtracking->DisableInterpolation(entity);

						if ( Vars::options.aim_resolver )
							resolver->resolve( entity );

						g_FakeLatency->UpdateIncomingSequences( FRAME_NET_UPDATE_POSTDATAUPDATE_START );

						//	OverrideYaw( );
					}

				}
				break;

			case FRAME_NET_UPDATE_POSTDATAUPDATE_END:

				break;

			case FRAME_RENDER_START:

				if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
					auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

					if ( !local_player )
						return;

					//--- Thirdperson Deadflag Stuff ---//
					if ( in_tp && Vars::options.aa_type > 0 )
						*( Vector* ) ( ( DWORD ) local_player + 0x31C8 ) = GLOBAL::real_angles;
					sdk::ConVar* recoil = ctx::convar->FindVar( "weapon_recoil_view_punch_extra" );
					if ( Vars::options.novisual_recoil ) {
						recoil->SetValueChar( "0" );
						pAimPunch = local_player->GetPunchAngles( );
						pViewPunch = local_player->GetPunchView( );
						vecAimPunch = *pAimPunch;
						vecViewPunch = *pViewPunch;
						*pAimPunch = Vector( 0, 0, 0 );
						*pViewPunch = Vector( 0, 0, 0 );
					}
					else if ( !Vars::options.novisual_recoil ) {
						recoil->SetValueChar( "0.055" );

					}
				}
				break;
			case FRAME_NET_UPDATE_START:
				if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
					if ( Vars::options.beam_bool )
						visuals->DrawBulletBeams( );
				}

				break;
			case FRAME_NET_UPDATE_END:

				if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
					for ( int i = 1; i < 65; i++ ) {
						auto entity = ctx::client_ent_list->GetClientEntity( i );
						auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

						if ( !entity )
							continue;

						if ( !local_player )
							continue;

						bool is_local_player = entity == local_player;
						bool is_teammate = local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player;

						if ( is_local_player )
							continue;

						if ( is_teammate )
							continue;

						if ( entity->GetHealth( ) <= 0 )
							continue;

						if ( Vars::options.aim_type == 0 )
							backtracking->DisableInterpolation( entity );
					}
				}

				break;
		}

		original_frame_stage_notify( ecx, stage );

		if ( pAimPunch && pViewPunch ) {
			*pAimPunch = vecAimPunch;
			*pViewPunch = vecViewPunch;
		}
	}
	void __fastcall HookedDrawModelExecute( void* ecx, void* edx, sdk::IMatRenderContext* context, const sdk::DrawModelState_t& state, const sdk::ModelRenderInfo_t& render_info, matrix3x4_t* matrix ) {
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
			for ( int i = 1; i < 65; i++ ) {
				auto entity = ctx::client_ent_list->GetClientEntity( i );
				auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

				if ( true ) {
					*( float* ) ( ( DWORD ) local_player + 0xA2FA ) = 0;
				}

				if ( !entity )
					continue;

				if ( !local_player )
					continue;


				if ( entity && entity->GetIsScoped( ) && in_tp && entity == local_player && !Vars::options.glow ) {
					ctx::render_view->SetBlend( 0.4 );
				}
			}
		}
		original_draw_model_execute( ecx, context, state, render_info, matrix );
	}
	void __fastcall HookedSceneEnd( void* ecx, void* edx ) {
		original_scene_end( ecx );

		static sdk::IMaterial* ignorez = chams->CreateMaterial( true, true, false );
		static sdk::IMaterial* notignorez = chams->CreateMaterial( false, true, false );
		static sdk::IMaterial* local = chams->CreateMaterial( false, true, false );

		static sdk::IMaterial* angles = chams->CreateMaterial( false, true, false );

		static sdk::IMaterial* ignorezf = chams->CreateMaterial( true, false, false );
		static sdk::IMaterial* notignorezf = chams->CreateMaterial( false, false, false );

		if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {
			auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
			for ( int i = 1; i < 65; i++ ) {
				auto entity = ctx::client_ent_list->GetClientEntity( i );


				if ( !entity )
					continue;

				if ( !local_player )
					continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player;

				if ( is_teammate )
					continue;

				if ( local_player && Vars::options.chams_local ) {
					local->ColorModulate( CColor( 255, 255, 255, 255 ) );
					ctx::render_view->SetColorModulationXD( CColor( 77, 137, 197 ) ); //255, 255, 255, 255
					ctx::model_render->ForcedMaterialOverride( local );
					local_player->DrawModel( 0x1, 255 );
					ctx::model_render->ForcedMaterialOverride( nullptr );
				}

				if ( local_player && Vars::options.aa_bool ) {
					/*	vec3_t OrigAng;
						OrigAng = local_player->get_eye_ang( );
						local_player->set_angles( vec3_t( 0, fakeangles.y, 0 ) );
						angles->AlphaModulate( 0 );
						ctx::model_render->ForcedMaterialOverride( angles );
						local_player->DrawModel( 0x1, 255 );
						ctx::model_render->ForcedMaterialOverride( nullptr );*/
				}

				if ( is_local_player )
					continue;

				//--- Colored Models ---//

				if ( entity && Vars::options.chams_type == 2 ) {


					ctx::model_render->ForcedMaterialOverride( ignorez );
					ignorez->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 77, 137, 197 ) ); //255, 40, 200
					entity->DrawModel( 0x1, 255 );

					ctx::model_render->ForcedMaterialOverride( notignorez );
					notignorez->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 167, 217, 77 ) ); //0, 125, 255
					entity->DrawModel( 0x1, 255 );
					ctx::model_render->ForcedMaterialOverride( nullptr );
				}
				else if ( entity && Vars::options.chams_type == 1 ) {
					ctx::model_render->ForcedMaterialOverride( notignorez );
					notignorez->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 167, 217, 77 ) ); //255, 40, 200
					entity->DrawModel( 0x1, 255 );
					ctx::model_render->ForcedMaterialOverride( nullptr );
				}
				else if ( entity && Vars::options.chams_type == 3 ) {
					ctx::model_render->ForcedMaterialOverride( notignorezf );
					notignorezf->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 167, 217, 77 ) ); //255, 40, 200		
					entity->DrawModel( 0x1, 255 );
					ctx::model_render->ForcedMaterialOverride( nullptr );
				}
				else if ( entity && Vars::options.chams_type == 4 ) {
					ctx::model_render->ForcedMaterialOverride( ignorezf );
					ignorezf->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 77, 137, 197 ) ); //255, 40, 200
					entity->DrawModel( 0x1, 255 );

					ctx::model_render->ForcedMaterialOverride( notignorezf );
					notignorezf->ColorModulate( CColor( 255, 255, 255, 255 ) ); //0, 125, 255
					ctx::render_view->SetColorModulationXD( CColor( 167, 217, 77 ) ); //0, 125, 255
					entity->DrawModel( 0x1, 255 );
					ctx::model_render->ForcedMaterialOverride( nullptr );
				}
			}

			//--- Wireframe Smoke ---//
			std::vector<const char*> vistasmoke_wireframe =
			{
				"particle/vistasmokev1/vistasmokev1_smokegrenade",
			};

			std::vector<const char*> vistasmoke_nodraw =
			{
				"particle/vistasmokev1/vistasmokev1_fire",
				"particle/vistasmokev1/vistasmokev1_emods",
				"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			};

			for ( auto mat_s : vistasmoke_wireframe ) {
				sdk::IMaterial* mat = ctx::material_system->FindMaterial( mat_s, TEXTURE_GROUP_OTHER );
				mat->SetMaterialVarFlag( sdk::MATERIAL_VAR_WIREFRAME, true ); //wireframe
			}

			for ( auto mat_n : vistasmoke_nodraw ) {
				sdk::IMaterial* mat = ctx::material_system->FindMaterial( mat_n, TEXTURE_GROUP_OTHER );
				mat->SetMaterialVarFlag( sdk::MATERIAL_VAR_NO_DRAW, true );
			}

			static auto smokecout = *( DWORD* ) ( linegoesthrusmoke + 0x8 );
			*( int* ) ( smokecout ) = 0;


			/*	auto pGlowObjectManager = *( sdk::CGlowObjectManager** )( UTILS::FindPattern( "client.dll", ( PBYTE )"\x0F\x11\x05\x00\x00\x00\x00\x83\xC8\x01", "xxx????xxx" ) + 3 );

				if ( !pGlowObjectManager || !Vars::options.glow )
					return;

				for ( int i = 0; i < pGlowObjectManager->GetSize( ); ++i ) {
					auto& glowObject = pGlowObjectManager->m_GlowObjectDefinitions [ i ];
					auto glowEntity = reinterpret_cast< sdk::CBaseEntity* >( glowObject.get_entity( ) );


					if ( glowObject.IsUnused( ) )
						continue;

					if ( !glowEntity )
						continue;

					if ( glowEntity->GetHealth( ) <= 0 )
						continue;

					bool is_local_player = glowEntity == local_player;

					if ( glowEntity->GetTeam( ) == local_player->GetTeam( ) && !is_local_player )
						continue;

					auto class_id = glowEntity->GetClientClass( )->m_ClassID;

					if ( class_id == 35 ) {
						glowObject.red = 197 / 255.f;
						glowObject.green = 77 / 255.f;
						glowObject.blue = 137 / 255.f;
						glowObject.alpha = 0.7f;
						glowObject.renderWhenOccluded = true;
						glowObject.renderWhenUnoccluded = false;
						if ( !is_local_player ) {
							glowObject.style = Vars::options.glow_style;
						}
						if ( is_local_player ) {
							glowObject.style = Vars::options.glow_style_local;
						}
					}

				}*/
		}
	}
	void dothirdperson( sdk::CViewSetup* pSetup ) {
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

		if ( !local_player )
			return;

		if ( local_player->GetIsScoped( ) )
			return;

		if ( in_tp && !local_player->GetIsScoped( ) )
			pSetup->fov = Vars::options.tp_distance;
		else if ( !in_tp && !local_player->GetIsScoped( ) )
			pSetup->fov = Vars::options.tp_distance - 20;

		if ( in_tp && local_player->GetHealth( ) <= 0 )
			in_tp = false;

		if ( !Vars::options.tp_bool )
			pSetup->fov = 110;

		if ( Vars::options.tp_bool && !in_tp )
			pSetup->fov = 110;

		if ( Vars::options.tp_bool && in_tp )
			pSetup->fov = Vars::options.tp_distance;
	}
	void __fastcall HookedOverrideView( void* ecx, void* edx, sdk::CViewSetup* pSetup ) {
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

		if ( !local_player )
			return;
		//--- Actual Thirdperson Stuff ---//
		if ( ctx::engine->IsConnected( ) && ctx::engine->IsInGame( ) ) {

			//auto svcheats = ctx::convar->FindVar("sv_cheats");
			//auto svcheatsspoof = new sdk::SpoofedConvar(svcheats);
			//svcheatsspoof->SetInt(1);

			auto GetCorrectDistance = [ &local_player ] ( float ideal_distance ) -> float {
				Vector inverse_angles;
				ctx::engine->GetViewAngles( inverse_angles );

				inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

				Vector direction;
				MATH::AngleVectors( inverse_angles, &direction );

				sdk::CTraceWorldOnly filter;
				sdk::trace_t trace;
				sdk::Ray_t ray;

				ray.Init( local_player->GetVecOrigin( ) + local_player->GetViewOffset( ), ( local_player->GetVecOrigin( ) + local_player->GetViewOffset( ) ) + ( direction * ( ideal_distance + 5.f ) ) );
				ctx::trace->TraceRay( ray, MASK_ALL, &filter, &trace );

				return ideal_distance * trace.flFraction;
			};

			if ( Vars::options.tp_bool && in_tp ) {
				if ( local_player->GetHealth( ) <= 0 )
					local_player->SetObserverMode( 5 );

				if ( !ctx::input->m_fCameraInThirdPerson ) {
					ctx::input->m_fCameraInThirdPerson = true;
					ctx::input->m_vecCameraOffset = Vector( GLOBAL::real_angles.x, GLOBAL::real_angles.y, GetCorrectDistance( 100 ) );
					Vector camForward;

					MATH::AngleVectors( Vector( ctx::input->m_vecCameraOffset.x, ctx::input->m_vecCameraOffset.y, 0 ), &camForward );
				}
			}
			else {
				ctx::input->m_fCameraInThirdPerson = false;
				ctx::input->m_vecCameraOffset = Vector( GLOBAL::real_angles.x, GLOBAL::real_angles.y, 0 );
			}
			sdk::ConVar* var = ctx::convar->FindVar( "weapon_debug_spread_show" );
			if ( !local_player->GetIsScoped( ) ) {
				var->SetValueChar( "3" );
			}
			else {
				var->SetValueChar( "0" );
			}

			sdk::ConVar* impacts = ctx::convar->FindVar( "sv_showimpacts" );
			if ( Vars::options.show_impact ) {
				impacts->SetValueChar( "1" );
			}
			else {
				impacts->SetValueChar( "0" );
			}

			dothirdperson( pSetup );
		}
		original_override_view( ecx, pSetup );
	}
	void __fastcall HookedTraceRay( void *thisptr, void*, const sdk::Ray_t &ray, unsigned int fMask, sdk::ITraceFilter *pTraceFilter, sdk::trace_t *pTrace ) {
		original_trace_ray( thisptr, ray, fMask, pTraceFilter, pTrace );
		pTrace->surface.flags |= SURF_SKY;
	}

	bool __fastcall HookedGetBool( void* pConVar, void* edx ) {
		if ( ( uintptr_t ) _ReturnAddress( ) == CAM_THINK )
			return true;

		return original_get_bool( pConVar );
	}
	float __fastcall HookedGetViewmodel( void* ecx, void* edx ) {
		static auto ofunc = HOOKS::override_view_hook_manager.GetOriginalFunction<get_fov_t>( 35 );
		float viewmodelFOV = ofunc( ecx );
		return Vars::options.vis_vfov;
	}
	long __stdcall hookedpresent( void* device, void* rect_source, void* rect_dest, void* window, void* dregion ) {
		gui::begin( );
		gui::render( );
		gui::end( );

		return directx_device_manager.GetOriginalFunction<PresentFn>( 17 )( device, rect_source, rect_dest, window, dregion );
	}
	long __stdcall hookedreset( void* device, void* present_parameters ) {
		auto or = directx_device_manager.GetOriginalFunction<ResetFn>( 16 );
		gui::destroy_objects( );

		auto hr = or ( device, present_parameters );
		if ( hr >= 0 ) {
			gui::create_objects( );
		}

		return hr;
	}
	void InitHooks( ) {
		auto device = **reinterpret_cast< void*** >( UTILS::FindSignature( "shaderapidx9.dll", "88 56 10" ) + 17 );

		directx_device_manager.Init( device );
		original_present = ( PresentFn ) directx_device_manager.HookFunction<PresentFn>( 17, hookedpresent );
		original_reset = ( ResetFn ) directx_device_manager.HookFunction<ResetFn>( 16, hookedreset );

		input_system_manager.Init( ctx::input_internal );
		o_SetMouseCodeState = reinterpret_cast< SetMouseCodeState_t >(
			input_system_manager.HookFunction<SetMouseCodeState_t>( 92, SetMouseCodeState_h ) );

		o_SetKeyCodeState = reinterpret_cast< SetKeyCodeState_t >(
			input_system_manager.HookFunction<SetKeyCodeState_t>( 91, SetKeyCodeState_h ) );

		iclient_hook_manager.Init( ctx::client );
		original_frame_stage_notify = reinterpret_cast< FrameStageNotifyFn >(
			iclient_hook_manager.HookFunction<FrameStageNotifyFn>( 36, HookedFrameStageNotify ) );

		panel_hook_manager.Init( ctx::panel );
		original_paint_traverse = reinterpret_cast< PaintTraverseFn >(
			panel_hook_manager.HookFunction<PaintTraverseFn>( 41, HookedPaintTraverse ) );

		model_render_hook_manager.Init( ctx::model_render );
		original_draw_model_execute = reinterpret_cast< DrawModelExecuteFn >( model_render_hook_manager.HookFunction<DrawModelExecuteFn>( 21, HookedDrawModelExecute ) );

		render_view_hook_manager.Init( ctx::render_view );
		original_scene_end = reinterpret_cast< SceneEndFn >( render_view_hook_manager.HookFunction<SceneEndFn>( 9, HookedSceneEnd ) );

		trace_hook_manager.Init( ctx::trace );
		original_trace_ray = reinterpret_cast< TraceRayFn >( trace_hook_manager.HookFunction<TraceRayFn>( 5, HookedTraceRay ) );

		override_view_hook_manager.Init( ctx::client_mode );
		original_override_view = reinterpret_cast< OverrideViewFn >( override_view_hook_manager.HookFunction<OverrideViewFn>( 18, HookedOverrideView ) );
		original_create_move = reinterpret_cast< CreateMoveFn >( override_view_hook_manager.HookFunction<CreateMoveFn>( 24, HookedCreateMove ) );
		o_vfov = reinterpret_cast< get_fov_t >( override_view_hook_manager.HookFunction<get_fov_t>( 35, HookedGetViewmodel ) );

		auto sv_cheats = ctx::convar->FindVar( "sv_cheats" );
		get_bool_manager = VMT::VMTHookManager( reinterpret_cast< DWORD** >( sv_cheats ) );
		original_get_bool = reinterpret_cast< SvCheatsGetBoolFn >( get_bool_manager.HookFunction<SvCheatsGetBoolFn>( 13, HookedGetBool ) );



		/* fakelatency works fine. */

	//	while ( !ctx::engine->IsInGame( ) || !ctx::engine->IsConnected( ) )
	//		Sleep( 1000 );

	//	while ( !( *ctx::client_state )->m_NetChannel )
	//		Sleep( 100 );

	//	net_channel_hook_manager.Init( ( DWORD** ) ( *ctx::client_state )->m_NetChannel );
	//	original_send_datagram = reinterpret_cast< SendDatagramFn >( net_channel_hook_manager.HookFunction<SendDatagramFn>( 48, hkSendDatagram ) );
	}

	void KillHooks( ) {
		input_system_manager.Restore( );
		iclient_hook_manager.Restore( );
		panel_hook_manager.Restore( );
		model_render_hook_manager.Restore( );
		render_view_hook_manager.Restore( );
		trace_hook_manager.Restore( );
		override_view_hook_manager.Restore( );
		get_bool_manager.Restore( );
		directx_device_manager.Restore( );
	//	net_channel_hook_manager.Restore( );
	}

	void EyeAnglesPitchHook( const sdk::CRecvProxyData *pData, void *pStruct, void *pOut ) {
		*reinterpret_cast< float* >( pOut ) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast< sdk::CBaseEntity* >( pStruct );
		if ( !entity )
			return;

	}
	void EyeAnglesYawHook( const sdk::CRecvProxyData *pData, void *pStruct, void *pOut ) {
		*reinterpret_cast< float* >( pOut ) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast< sdk::CBaseEntity* >( pStruct );
		if ( !entity )
			return;

		//resolver->record(entity, pData->m_Value.m_Float);
	}

	void InitNetvarHooks( ) {
		UTILS::netvar_hook_manager.Hook( "DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook );
		UTILS::netvar_hook_manager.Hook( "DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook );
	}
}