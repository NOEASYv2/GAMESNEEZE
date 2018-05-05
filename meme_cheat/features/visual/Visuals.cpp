#include "../../includes.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/IEngine.h"
#include "../../sdk/CUserCmd.h"
#include "../../sdk/ConVar.h"
#include "../../sdk/CGlobalVars.h"
#include "../../sdk/IViewRenderBeams.h"
#include "../lag compensation/Backtracking.h"
#include "../../sdk/CBaseEntity.h"
#include "../../sdk/CClientEntityList.h"
#include "../../sdk/CBaseWeapon.h"
#include "../../sdk/CTrace.h"	
#include "../resolver/Resolver.h"
#include "../../sdk/CGlobalVars.h"
#include "../visual/Visuals.h"
#include "../../utilities/render.h"
#include "../../sdk/IVDebugOverlay.h"
#include "../../sdk/Collideable.h"
#include "../autowall/AutoWall.h"
#include <string.h>

//--- Misc Variable Initalization ---//
int alpha [ 65 ];
int health_alpha [ 65 ];
CColor breaking;
CColor backtrack;
CColor update;
static bool bPerformed = false, bLastSetting;
float fade_alpha [ 65 ];
float dormant_time [ 65 ];
CColor main_color;
CColor ammo;

inline void CrossProduct( const Vector& a, const Vector& b, Vector& result ) {
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

__forceinline float VecDotProduct( const Vector& a, const Vector& b ) {
	return ( a.x * b.x + a.y * b.y + a.z * b.z );
}

bool CVisuals::GetBox( sdk::CBaseEntity* pEntity, CVisuals::ESPBox &result ) {
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetVecOrigin( );
	min = pEntity->GetCollideable( )->Mins( ) + vOrigin;
	max = pEntity->GetCollideable( )->Maxs( ) + vOrigin;

	// Points of a 3d bounding box
	Vector points [ ] = { Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( max.x, min.y, max.z ) };

	// Get screen positions
	if ( !RENDER::WorldToScreen( points [ 3 ], flb ) || !RENDER::WorldToScreen( points [ 5 ], brt )
		|| !RENDER::WorldToScreen( points [ 0 ], blb ) || !RENDER::WorldToScreen( points [ 4 ], frt )
		|| !RENDER::WorldToScreen( points [ 2 ], frb ) || !RENDER::WorldToScreen( points [ 1 ], brb )
		|| !RENDER::WorldToScreen( points [ 6 ], blt ) || !RENDER::WorldToScreen( points [ 7 ], flt ) )
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr [ ] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for ( int i = 1; i < 8; i++ ) {
		if ( left > arr [ i ].x )
			left = arr [ i ].x;
		if ( bottom < arr [ i ].y )
			bottom = arr [ i ].y;
		if ( right < arr [ i ].x )
			right = arr [ i ].x;
		if ( top > arr [ i ].y )
			top = arr [ i ].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return true;
}

RECT CVisuals::GetBBox( sdk::CBaseEntity* entity, Vector pointstransf [ ] ) {
	RECT rect { };
	auto collideable = entity->GetCollideable( );

	if ( !collideable )
		return rect;

	auto min = collideable->Mins( );
	auto max = collideable->Maxs( );

	const matrix3x4_t &trans = entity->m_rgflCoordinateFrame( );

	Vector points [ ] =
	{
		Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( max.x, min.y, max.z )
	};

	Vector pointsTransformed [ 8 ];
	for ( int i = 0; i < 8; i++ ) {
		MATH::VectorTransform( points [ i ], trans, pointsTransformed [ i ] );
	}

	Vector pos = entity->GetVecOrigin( );
	Vector screen_points [ 8 ] = { };

	for ( int i = 0; i < 8; i++ )
		if ( !RENDER::WorldToScreen( pointsTransformed [ i ], screen_points [ i ] ) )
			return rect;
		else
			pointstransf [ i ] = screen_points [ i ];

	auto left = screen_points [ 0 ].x;
	auto top = screen_points [ 0 ].y;
	auto right = screen_points [ 0 ].x;
	auto bottom = screen_points [ 0 ].y;

	for ( int i = 1; i < 8; i++ ) {
		if ( left > screen_points [ i ].x )
			left = screen_points [ i ].x;
		if ( top < screen_points [ i ].y )
			top = screen_points [ i ].y;
		if ( right < screen_points [ i ].x )
			right = screen_points [ i ].x;
		if ( bottom > screen_points [ i ].y )
			bottom = screen_points [ i ].y;
	}
	return RECT { ( long ) left, ( long ) top, ( long ) right, ( long ) bottom };
}

void CVisuals::Draw( ) {
	Hitmarker( );
	for ( int i = 1; i < ctx::globals->maxclients; i++ ) {
		auto entity = ctx::client_ent_list->GetClientEntity( i );
		auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

		if ( !entity )
			continue;

		if ( !local_player )
			continue;

		bool is_local_player = entity == local_player;

		if ( Vars::options.localesp == false ) {
			if ( is_local_player )
				continue;
		}

		if ( local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player )
			continue;

		if ( entity->GetHealth( ) <= 0 )
			continue;

		//--- Colors ---//
		int enemy_hp = entity->GetHealth( );
		int hp_red = 255 - ( enemy_hp * 2.55 );
		int hp_green = enemy_hp * 2.55;
		CColor health_color = CColor( hp_red, hp_green, 1, alpha [ entity->GetIndex( ) ] );
		CColor dormant_color = CColor( 100, 100, 100, alpha [ entity->GetIndex( ) ] );
		CColor h_dormant_color = CColor( 0, 0, 0, health_alpha [ entity->GetIndex( ) ] );
		CColor resolve_color = CColor( 0, 175, 0, alpha [ entity->GetIndex( ) ] ); //resolve_color
		CColor box_color;
		CColor still_health;
		CColor alt_color;
		CColor zoom_color;
		CColor bomb_color;

		//--- Domant ESP Checks ---//
		if ( entity->GetIsDormant( ) ) {
			if ( alpha [ entity->GetIndex( ) ] > 0 )
				alpha [ entity->GetIndex( ) ] -= 0.4;

			if ( health_alpha [ entity->GetIndex( ) ] > 0 )
				health_alpha [ entity->GetIndex( ) ] -= 0.4;
			main_color = dormant_color;
			still_health = dormant_color;
			alt_color = dormant_color;
			zoom_color = dormant_color;
			breaking = dormant_color;
			backtrack = dormant_color;
			resolve_color = dormant_color;
			box_color = dormant_color;
			bomb_color = dormant_color;
			ammo = dormant_color;
		}
		else if ( !entity->GetIsDormant( ) ) {
			alpha [ entity->GetIndex( ) ] = 255;
			health_alpha [ entity->GetIndex( ) ] = 190;
			main_color = CColor( 255, 255, 255, alpha [ entity->GetIndex( ) ] ); //heath_color
			resolve_color = CColor( 0, 175, 0, alpha [ entity->GetIndex( ) ] ); //resolve_color
			still_health = health_color;
			alt_color = BLACK;
			zoom_color = CColor( 150, 150, 220, 255 );
			breaking = CColor( 220, 150, 150 );
			backtrack = CColor( 155, 220, 150 );
			box_color = CColor( 255, 255, 255 );
			bomb_color = CColor( 244, 66, 66, 150 );
			ammo = CColor( 61, 135, 255 );
		}

		//--- Entity Related Rendering ---///
		if ( Vars::options.box_bool ) DrawBox( entity, box_color );
		if ( Vars::options.name_bool ) DrawName( entity, main_color, i );
		if ( Vars::options.weap_bool ) DrawWeapon( entity, main_color, i );
		if ( Vars::options.info_bool ) DrawInfo( entity, main_color, zoom_color );
		if ( Vars::options.distance_bool )DrawDistance( entity, main_color );
		if ( Vars::options.health_bool ) DrawHealth( entity, still_health, h_dormant_color );
		if ( Vars::options.offscreen ) OffScreen( entity );
		if ( Vars::options.resolve ) DrawResolveMode( entity, resolve_color, i );
		if ( Vars::options.show_spread ) ShowSpread( );
		if ( Vars::options.speclist ) DrawSpectators( local_player );
		if ( Vars::options.draw_bt ) DrawBacktrack( );
		if ( Vars::options.dmg_bool )DrawDamageIndicator( );
		
		DrawSounds( );
		DrawLogs( );

		//--- Local Related Rendering ---///
		if ( is_local_player && Vars::options.box_bool_local ) {
			if ( Vars::options.box_bool ) DrawBox( entity, box_color );
			if ( Vars::options.name_bool ) DrawName( entity, main_color, i );
			if ( Vars::options.weap_bool ) DrawWeapon( entity, main_color, i );
			if ( Vars::options.info_bool ) DrawInfo( entity, main_color, zoom_color );
			if ( Vars::options.health_bool ) DrawHealth( entity, still_health, h_dormant_color );
		}
	}
}

void CVisuals::ClientDraw( ) {
	DrawBorderLines( );
	DrawIndicator( );
	ModulateWorld( );
	DrawHitmarker( );
	AsusProps( );
}

void CVisuals::OffScreen( sdk::CBaseEntity* entity ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	auto entityPos = entity->GetVecOrigin( );

	Vector outputkurwa;

	if ( RENDER::WorldToScreen( entityPos, outputkurwa ) )
		return;

	if ( Vars::options.box_bool_local == false )
		return;

	Vector eyeangles, poopvec;
	int screen_w, screen_h;
	ctx::engine->GetScreenSize( screen_w, screen_h );
	ctx::engine->GetViewAngles( eyeangles );
	QAngle newangle = MATH::CalcAngle( Vector( local_player->GetAbsOrigin( ).x, local_player->GetAbsOrigin( ).y, 0 ), Vector( entity->GetAbsOrigin( ).x, entity->GetAbsOrigin( ).y, 0 ) );
	MATH::AngleVectors( QAngle( 0, 270, 0 ) - newangle + QAngle( 0, eyeangles.y, 0 ), &poopvec );
	auto circlevec = Vector( screen_w / 2, screen_h / 2, 0 ) + ( poopvec * 250.f );
	RENDER::DrawFilledCircle( circlevec.x, circlevec.y, Vars::options.offscreenballsize, 10, entity->GetIsDormant( ) ? CColor( 255, 0, 0, 100 ) : CColor( 255, 0, 0, 255 ) );
}

void CVisuals::AsusWalls( ) {

}

void CVisuals::AsusProps( ) {
	static bool asuswalls_performed = false, asus_lastsetting;
	static float original_value = Vars::options.asus_props_o;

	if ( !ctx::engine->IsConnected( ) || !ctx::engine->IsInGame( ) ) {
		if ( asuswalls_performed )
			asuswalls_performed = false;
		return;
	}

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( !local_player ) {
		if ( asuswalls_performed )
			asuswalls_performed = false;
		return;
	}

	if ( asus_lastsetting != Vars::options.asus_props ) {
		asus_lastsetting = Vars::options.asus_props;
		asuswalls_performed = false;
	}

	if ( !asuswalls_performed ) {
		static sdk::ConVar* staticdrop = ctx::convar->FindVar( "r_DrawSpecificStaticProp" );
		staticdrop->SetValue( 0 );

		for ( sdk::MaterialHandle_t i = ctx::material_system->FirstMaterial( ); i != ctx::material_system->InvalidMaterial( ); i = ctx::material_system->NextMaterial( i ) ) {
			sdk::IMaterial *pmat = ctx::material_system->GetMaterial( i );

			if ( !pmat )
				continue;

			if ( strstr( pmat->GetMaterialName( ), "crate" ) ||
				strstr( pmat->GetMaterialName( ), "box" ) ||
				strstr( pmat->GetMaterialName( ), "door" ) ) {
				if ( Vars::options.asus_props ) {
					pmat->AlphaModulate( 0.7f );
					pmat->SetMaterialVarFlag( sdk::MATERIAL_VAR_TRANSLUCENT, true );
				}
				else {
					pmat->SetMaterialVarFlag( sdk::MATERIAL_VAR_TRANSLUCENT, false );
					pmat->AlphaModulate( 1.0f );
				}
			}

		}
		asuswalls_performed = true;
	}

}

void CVisuals::ShowSpread( ) {
	auto local = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	auto wpn = ctx::client_ent_list->GetClientEntity( local->GetActiveWeaponIndex( ) );
	int w, h;
	ctx::engine->GetScreenSize( w, h );
	int x, y;
	x = w / 2;
	y = h / 2;
	auto accuracy = wpn->GetInaccuracy( ) * 550.f; //3000

	CColor color = CColor(
		Vars::options.spread_color.RGBA [ 0 ],
		Vars::options.spread_color.RGBA [ 1 ],
		Vars::options.spread_color.RGBA [ 2 ],
		Vars::options.spread_alpha );

	if ( local && local->GetHealth( ) > 0 )
		RENDER::DrawFilledCircle( Vector2D( x, y ), color, accuracy, 100 );

}

std::string str_to_upper( std::string strToConvert ) {
	std::transform( strToConvert.begin( ), strToConvert.end( ), strToConvert.begin( ), ::toupper );

	return strToConvert;
}

void DrawHitboxes( sdk::CBaseEntity* pEntity, int r, int g, int b, int a, float duration ) {
	matrix3x4_t matrix [ 128 ];

	if ( !pEntity->SetupBones( matrix, 128, 0x00000100, 0 ) )
		return;

	sdk::studiohdr_t* hdr = ctx::model_info->GetStudioModel( pEntity->GetModel( ) );
	sdk::mstudiohitboxset_t* set = hdr->GetHitboxSet( 0 );

	for ( int i = 0; i < set->numhitboxes; i++ ) {
		sdk::mstudiobbox_t* hitbox = set->GetHitbox( i );

		if ( !hitbox )
			continue;

		Vector vMin, vMax;
		MATH::VectorTransform( hitbox->bbmin, matrix [ hitbox->bone ], vMin );
		MATH::VectorTransform( hitbox->bbmax, matrix [ hitbox->bone ], vMax );

		ctx::debug_overlay->DrawPill( vMin, vMax, hitbox->m_flRadius /*box + 0x30*/, r, g, b, a, duration );
	}
}

void CVisuals::Hitmarker( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	int center_width;
	int center_height;
	ctx::engine->GetScreenSize( center_width, center_height );

	center_width = center_width / 2;
	center_height = center_height / 2;

	if ( hitmarker_time > 0 ) {
		auto line_size = 10;

		RENDER::DrawLine( center_width - line_size, center_height - line_size, center_width - ( line_size / 2 ), center_height - ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width - line_size, center_height + line_size, center_width - ( line_size / 2 ), center_height + ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width + line_size, center_height + line_size, center_width + ( line_size / 2 ), center_height + ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width + line_size, center_height - line_size, center_width + ( line_size / 2 ), center_height - ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		hitmarker_time -= 1;
	}
	if ( hitmarker_time < 0 ) {

	}
}

void CVisuals::DrawBox( sdk::CBaseEntity* entity, CColor color ) {
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;

		RENDER::DrawEmptyRect( pos.x - width / 2, top.y, ( pos.x - width / 2 ) + width, top.y + height, color );
		RENDER::DrawEmptyRect( ( pos.x - width / 2 ) + 1, top.y + 1, ( pos.x - width / 2 ) + width - 1, top.y + height - 1, CColor( 20, 20, 20, alpha [ entity->GetIndex( ) ] ) );
		RENDER::DrawEmptyRect( ( pos.x - width / 2 ) - 1, top.y - 1, ( pos.x - width / 2 ) + width + 1, top.y + height + 1, CColor( 20, 20, 20, alpha [ entity->GetIndex( ) ] ) );
	}
}

void CVisuals::localDrawBox( sdk::CBaseEntity* entity, CColor color ) {

}

void CVisuals::NewBox( sdk::CBaseEntity* entity, CColor color ) { }

void CVisuals::DrawCorners( sdk::CBaseEntity* entity, CColor color ) {
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;

		RENDER::DrawEdges( pos.x - width / 2, top.y, ( pos.x - width / 2 ) + width, top.y + height, 8, color );
		RENDER::DrawEdges( ( pos.x - width / 2 ) + 1, top.y + 1, ( pos.x - width / 2 ) + width - 1, top.y + height - 1, 8, CColor( 20, 20, 20, alpha [ entity->GetIndex( ) ] ) );
		RENDER::DrawEdges( ( pos.x - width / 2 ) - 1, top.y - 1, ( pos.x - width / 2 ) + width + 1, top.y + height + 1, 8, CColor( 20, 20, 20, alpha [ entity->GetIndex( ) ] ) );
	}
}

void CVisuals::DrawName( sdk::CBaseEntity* entity, CColor color, int index ) {
	sdk::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );
	ctx::engine->GetPlayerInfo( index, &ent_info );

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;
		RENDER::DrawF( pos.x, top.y - 7, FONTS::visuals_name_font, true, true, color, ent_info.name ); //numpad_menu_font
	}
}

float CVisuals::resolve_distance( Vector src, Vector dest ) {
	Vector delta = src - dest;

	float fl_dist = ::sqrtf( ( delta.Length( ) ) );

	if ( fl_dist < 1.0f )
		return 1.0f;

	return fl_dist;
}

void CVisuals::DrawDistance( sdk::CBaseEntity* entity, CColor color ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	sdk::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );
	Vector vecOrigin = entity->GetVecOrigin( );
	Vector vecOriginLocal = local_player->GetVecOrigin( );

	char dist_to [ 32 ];
	sprintf_s( dist_to, "%.0f ft", resolve_distance( vecOriginLocal, vecOrigin ) );


	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;
		RENDER::DrawF( pos.x, pos.y + 8, FONTS::visuals_esp_font, true, true, color, dist_to );
	}
}

std::string fix_item_name( std::string name ) {
	std::string cname = name;

	if ( cname [ 0 ] == 'C' )
		cname.erase( cname.begin( ) );

	auto startOfWeap = cname.find( "Weapon" );
	if ( startOfWeap != std::string::npos )
		cname.erase( cname.begin( ) + startOfWeap, cname.begin( ) + startOfWeap + 6 );

	return cname;
}

std::string revolver( std::string name ) {
	std::string cname = name;
	std::string revolver = "R8 Revolver";

	if ( cname [ 0 ] == 'C' )
		cname.erase( cname.begin( ) );

	auto startOfWeap = cname.find( "" );
	if ( startOfWeap != std::string::npos )
		cname.erase( revolver.begin( ) + startOfWeap, revolver.begin( ) + startOfWeap + 6 );

	return cname;
}

void CVisuals::DrawAmmo( sdk::CBaseEntity* entity, CColor color, int index ) {

}

void CVisuals::DrawWeapon( sdk::CBaseEntity* entity, CColor color, int index ) {
	sdk::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );
	ctx::engine->GetPlayerInfo( index, &ent_info );

	auto weapon = ctx::client_ent_list->GetClientEntity( entity->GetActiveWeaponIndex( ) );
	auto c_baseweapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( entity->GetActiveWeaponIndex( ) ) );

	if ( !c_baseweapon )
		return;


	if ( !weapon )
		return;

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;

		switch ( Vars::options.wep_type ) {
			case 0:
				if ( c_baseweapon->isrevolver( ) ) {
					RENDER::DrawF( pos.x, pos.y + 16, FONTS::visuals_esp_font, true, true, color, "r8 revolver (" + std::to_string( c_baseweapon->GetLoadedAmmo( ) ) + ")" );
				}
				else {
					if ( c_baseweapon->isknife_entity( ) )
						RENDER::DrawF( pos.x, pos.y + 16, FONTS::visuals_esp_font, true, true, color, fix_item_name( weapon->GetClientClass( )->m_pNetworkName ) );
					else
						RENDER::DrawF( pos.x, pos.y + 16, FONTS::visuals_esp_font, true, true, color, fix_item_name( weapon->GetClientClass( )->m_pNetworkName )
						+ " (" + std::to_string( c_baseweapon->GetLoadedAmmo( ) ) + ")" );
				}
				break;
			case 1:
				RENDER::DrawF( pos.x, pos.y + 19, FONTS::visuals_weapon_icon_font, true, true, color, c_baseweapon->GetGunIcon( ) );
				break;
		}
	}
}

void CVisuals::DrawResolveMode( sdk::CBaseEntity* entity, CColor color, int index ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	sdk::player_info_t ent_info;
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );
	ctx::engine->GetPlayerInfo( index, &ent_info );

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;

		int twoja_stara = 25;
		int twoja_stara2 = 25;
		if ( Vars::options.weap_bool ) {
			twoja_stara = 25;
			twoja_stara2 = 32;
		}
		else if ( !Vars::options.weap_bool ) {
			twoja_stara = 18;
			twoja_stara2 = 25;
		}

		RENDER::DrawF( pos.x, pos.y + twoja_stara, FONTS::visuals_esp_font, true, true, color, GLOBAL::current_resolve );
	}
}
template <typename t> t xdd( t value, t min, t max ) {
	if ( value > max ) {
		return max;
	}
	if ( value < min ) {
		return min;
	}
	return value;
}
long GetEpochTime( ) {
	using namespace std::chrono;
	milliseconds ms = duration_cast< milliseconds >(
		system_clock::now( ).time_since_epoch( )
		);

	return ms.count( );
}
void CVisuals::CollectFootstep( int iEntIndex, const char *pSample ) {
	if ( strstr( pSample, "player/footsteps" ) == NULL && strstr( pSample, "player/land" ) == NULL )
		return;

	if ( iEntIndex == ctx::engine->GetLocalPlayer( ) )
		return;

	Footstep footstep;
	footstep.entityId = iEntIndex;
	footstep.position = ctx::client_ent_list->GetClientEntity( iEntIndex )->GetVecOrigin( );

	footstep.expiration = GetEpochTime( ) + 1000;

	footsteps.push_back( footstep );
}
void CVisuals::DrawSounds( ) {
	for ( unsigned int i = 0; i < footsteps.size( ); i++ ) {
		long diff = footsteps [ i ].expiration - GetEpochTime( );

		if ( diff <= 0 ) {
			footsteps.erase( footsteps.begin( ) + i );
			continue;
		}

		Vector pos2d;

		if ( ctx::debug_overlay->ScreenPosition( footsteps [ i ].position, pos2d ) )
			continue;

		auto localplayer = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
		if ( !localplayer )
			continue;

		auto player = ctx::client_ent_list->GetClientEntity( footsteps [ i ].entityId );
		if ( !player )
			continue;

		if ( player->GetTeam( ) == localplayer->GetTeam( ) )
			continue;

		float percent = ( float ) diff / ( float ) 1000;

		CColor playerColor = Vars::options.menu_col;
		Vars::options.menu_col.RGBA [ 3 ] = min( powf( percent * 2, 0.6f ), 1.f ) * Vars::options.menu_col.RGBA [ 3 ]; // fades out alpha when its below 0.5

		float circleRadius = fabs( percent - 1.f ) * 42.f;
		float points = circleRadius * 0.75f;

		RENDER::Circle3D( footsteps [ i ].position, points, circleRadius, playerColor );
	}
}
void CVisuals::EmitSound( int iEntIndex, const char *pSample ) {
	//	if ( pSample && ctx::engine->IsInGame( ) )
	//		CollectFootstep( iEntIndex, pSample );
}

void CVisuals::DrawHealth( sdk::CBaseEntity* entity, CColor color, CColor dormant ) {
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );

	int enemy_hp = xdd( entity->GetHealth( ), 0, 100 );
	int hp_red = 255 - ( enemy_hp * 2.55 );
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor( hp_red, hp_green, 1, alpha [ entity->GetIndex( ) ] );

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;

		float offset = ( height / 4.f ) + 5;
		UINT hp = height - ( UINT ) ( ( height * enemy_hp ) / 100 );

		//RENDER::DrawEmptyRect( ( pos.x - width / 2 ) - 6, top.y - 1, ( pos.x - width / 2 ) - 3, top.y + height + 1, dormant ); //intense maths
		RENDER::DrawLine( ( pos.x - width / 2 ) - 4, top.y, ( pos.x - width / 2 ) - 4, top.y + height, dormant ); //could have done a rect here,
		RENDER::DrawLine( ( pos.x - width / 2 ) - 5, top.y, ( pos.x - width / 2 ) - 5, top.y + height, dormant ); //but fuck it
		RENDER::DrawLine( ( pos.x - width / 2 ) - 4, top.y + hp, ( pos.x - width / 2 ) - 4, top.y + height, color ); //could have done a rect here,
		RENDER::DrawLine( ( pos.x - width / 2 ) - 5, top.y + hp, ( pos.x - width / 2 ) - 5, top.y + height, color ); //but fuck it
		if ( entity->GetHealth( ) < 100 )
			RENDER::DrawF( ( pos.x - width / 2 ) - 4, top.y + hp, FONTS::visuals_esp_font, true, true, CColor( 255, 255, 255, alpha[ entity->GetIndex( )] ), std::to_string( enemy_hp ) );
	}
}

void CVisuals::DrawAmmo( sdk::CBaseEntity* entity, CColor color, CColor dormant ) {
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );

	int enemy_hp = xdd( entity->GetHealth( ), 0, 100 );
	int hp_red = 255 - ( enemy_hp * 2.55 );
	int hp_green = enemy_hp * 2.55;
	CColor health_color = CColor( hp_red, hp_green, 1, alpha [ entity->GetIndex( ) ] );

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	bool is_local_player = entity == local_player;
	bool is_teammate = local_player->GetTeam( ) == entity->GetTeam( ) && !is_local_player;

	if ( is_local_player )
		return;

	if ( is_teammate )
		return;

	auto c_baseweapon = reinterpret_cast< sdk::CBaseWeapon* >( ctx::client_ent_list->GetClientEntity( entity->GetActiveWeaponIndex( ) ) );

	if ( !c_baseweapon )
		return;

	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		//int width = height / 2;

		float offset = ( height / 4.f ) + 5;
		UINT hp = height - ( UINT ) ( ( height * 3 ) / 100 );

		auto animLayer = local_player->GetAnimOverlay( 1 );
		if ( !animLayer.m_pOwner )
			return;

		auto activity = local_player->GetSequenceActivity( animLayer.m_nSequence );

		int iClip = c_baseweapon->GetLoadedAmmo( );
		int iClipMax = c_baseweapon->get_full_info( )->iMaxClip1;

		float box_w = ( float ) fabs( height / 2 );
		float width;
		if ( activity == 967 && animLayer.m_flWeight != 0.f ) {
			float cycle = animLayer.m_flCycle; // 1 = finished 0 = just started
			width = ( ( ( box_w * cycle ) / 1.f ) );
		}
		else
			width = ( ( ( box_w * iClip ) / iClipMax ) );


		RENDER::DrawFilledRect( ( pos.x - box_w / 2 ) - 6, top.y - 1, ( pos.x - box_w / 2 ) - 3, top.y + height + 1, dormant );
		RENDER::DrawFilledRect( ( pos.x - width / 2 ) - 6, top.y, ( pos.x - width / 2 ) - 3, top.y + height, CColor( 0, 75, 255 ) );
	}
}

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

void CVisuals::DrawFlags( sdk::CBaseEntity* entity, CColor color, CColor alt ) {

}
void CVisuals::DrawAnimations( sdk::CBaseEntity* entity, CColor color, CColor alt ) {

}

void CVisuals::DrawInfo( sdk::CBaseEntity* entity, CColor color, CColor alt ) {
	std::vector<std::pair<std::string, CColor>> stored_info;
	std::vector<std::pair<std::string, CColor>> local_info;
	Vector min, max;
	entity->GetRenderBounds( min, max );
	Vector pos, pos3D, top, top3D;
	pos3D = entity->GetAbsOrigin( ) - Vector( 0, 0, 10 );
	top3D = pos3D + Vector( 0, 0, max.z + 10 );

	//if (entity->GetHealth() > 0)
		//stored_info.push_back(std::pair<std::string, CColor>("hp: " + std::to_string(entity->GetHealth()), color));

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( Vars::options.flags_bool ) {
		if ( entity->GetArmor( ) > 0 )
			stored_info.push_back( std::pair<std::string, CColor>( entity->GetArmorName( ), color ) );
	}

	for ( int s = 0; s < 13; s++ ) {
		auto anim_layer = entity->GetAnimOverlay( s );
		if ( Vars::options.current_cycle ) {
			if ( anim_layer.m_nSequence > 0 )
				stored_info.push_back( std::pair<std::string, CColor>( "activity: " + std::to_string( ( sdk::CSGO_ACTS ) anim_layer.m_nSequence ), CColor( 255, 255, 255, alpha [ entity->GetIndex( ) ] ) ) );
		}

	}
	for ( int s = 0; s < 13; s++ ) {
		auto anim_layer = entity->GetAnimOverlay( s );
		if ( Vars::options.current_cycle_local ) {
			if ( anim_layer.m_nSequence > 0 )
				local_info.push_back( std::pair<std::string, CColor>( "activity: " + std::to_string( ( sdk::CSGO_ACTS )anim_layer.m_nSequence ), CColor( 255, 255, 255, 255 ) ) );
		}
	}

	if ( Vars::options.zoom ) {
		if ( entity->GetIsScoped( ) )
			stored_info.push_back( std::pair<std::string, CColor>( "zoom", CColor( 0, 125, 255, alpha [ entity->GetIndex( ) ] ) ) );
	}

	if ( Vars::options.money )
		stored_info.push_back( std::pair<std::string, CColor>( "$" + std::to_string( entity->GetMoney( ) ), CColor( 0, 175, 0, alpha [ entity->GetIndex( ) ] ) ) );

	if ( Vars::options.fake ) {
		if ( !using_fake_angles [ entity->GetIndex( ) ] )
			stored_info.push_back( std::pair<std::string, CColor>( "fake", color ) );
	}

	static float LBY [ 64 ];
	static float time [ 64 ];

	float lby = entity->GetLowerBodyYaw( );

	int Index = entity->GetIndex( );

	if ( lby != LBY [ Index ] || ( entity->GetVelocity( ).Length2D( ) > 0.1f && entity->GetFlags( ) & FL_ONGROUND ) ) {
		LBY [ Index ] = lby;
		time [ Index ] = ctx::globals->curtime;
	}

	if ( Vars::options.lbyupdate )
		stored_info.push_back( std::pair<std::string, CColor>( "last update: " + std::to_string( ctx::globals->curtime - time [ entity->GetIndex( ) ] ), CColor( 0, 175, 0, alpha [ entity->GetIndex( ) ] ) ) );

	if ( Vars::options.velocity ) {
		if ( entity->GetVelocity( ).Length2D( ) > 36 )
			stored_info.push_back( std::pair<std::string, CColor>( "velocity: " + std::to_string( entity->GetVelocity( ).Length2D( ) ), CColor( 0, 255, 0, alpha [ entity->GetIndex( ) ] ) ) );
		else if ( entity->GetVelocity( ).Length2D( ) < 36 && entity->GetVelocity( ).Length2D( ) > 0.1 )
			stored_info.push_back( std::pair<std::string, CColor>( "velocity: " + std::to_string( entity->GetVelocity( ).Length2D( ) ), CColor( 255, 255, 0, alpha [ entity->GetIndex( ) ] ) ) );
		else
			stored_info.push_back( std::pair<std::string, CColor>( "velocity: " + std::to_string( entity->GetVelocity( ).Length2D( ) ), CColor( 255, 0, 0, alpha [ entity->GetIndex( ) ] ) ) );

	}

	std::string act;

	int height = ( pos.y - top.y );
	int width = height / 2;
	int i = 0;
	for ( auto local_Text : local_info ) {
		RENDER::DrawF( ( pos.x + width / 2 ) + 5, top.y + i, FONTS::visuals_esp_font, false, false, local_Text.second, local_Text.first ); //numpad_menu_font
		i += 8;
	}


	if ( RENDER::WorldToScreen( pos3D, pos ) && RENDER::WorldToScreen( top3D, top ) ) {
		int height = ( pos.y - top.y );
		int width = height / 2;
		int i = 0;
		for ( auto Text : stored_info ) {
			RENDER::DrawF( ( pos.x + width / 2 ) + 5, top.y + i, FONTS::visuals_esp_font, false, false, Text.second, Text.first ); //numpad_menu_font
			i += 8;
		}
	}
}

void CVisuals::DrawSpectators( sdk::CBaseEntity *local ) {
	int w, h;

	RECT scrn = ctx::engine->GetViewport( );
	int kapi = 0;

	/*if ( local ) {
		for ( int i = 1; i < ctx::client_ent_list->GetHighestEntityIndex(); i++ ) {
			// Get the entity
			sdk::CBaseEntity *pEntity = ctx::client_ent_list->GetClientEntity( i );
			sdk::player_info_t pinfo;
			if ( pEntity && pEntity != local ) {
				if ( ctx::engine->GetPlayerInfo( i, &pinfo ) && !pEntity->GetHealth( ) > 0 && !pEntity->GetIsDormant( ) ) {
					HANDLE obs = pEntity->GetObserverModeHandle( );
					if ( obs ) {
						sdk::CBaseEntity *pTarget = ctx::client_ent_list->GetClientEntityFromHandle( obs );
						sdk::player_info_t pinfo2;
						if ( pTarget == local ) {
							if ( ctx::engine->GetPlayerInfo( pTarget->GetIndex( ), &pinfo2 ) ) {
								RENDER::DrawF( scrn.left + 98, ( scrn.top ) + ( 16 * kapi ) + 20, FONTS::visuals_esp_font, true, true, CColor(255,255,255,255), pinfo.name );
								kapi++;
							}
						}
					}
				}
			}
		}
	}*/
}

void CVisuals::DrawSkeleton( sdk::CBaseEntity* entity, CColor color ) //wut hek
{
	matrix3x4_t bone_matrix [ 128 ];
	if ( !entity->SetupBones( bone_matrix, 128, BONE_USED_BY_ANYTHING, 0 ) )
		return;

	auto studio_model = ctx::model_info->GetStudioModel( entity->GetModel( ) );
	if ( !studio_model )
		return;

	for ( int i = 0; i < studio_model->numbones; i++ ) {
		auto bone = studio_model->GetBone( i );
		if ( !bone || bone->parent < 0 || !( bone->flags & BONE_USED_BY_HITBOX ) )
			continue;

		/// 2 bone matrices, 1 for the child and 1 for the parent bone
		matrix3x4_t bone_matrix_1, bone_matrix_2;
		if ( bone_matrix ) {
			bone_matrix_1 = bone_matrix [ i ];
			bone_matrix_2 = bone_matrix [ bone->parent ];
		}
		else {
			bone_matrix_1 = entity->GetBoneMatrix( i );
			bone_matrix_2 = entity->GetBoneMatrix( bone->parent );
		}

		Vector bone_position_1 = Vector( bone_matrix_1 [ 0 ] [ 3 ], bone_matrix_1 [ 1 ] [ 3 ], bone_matrix_1 [ 2 ] [ 3 ] ),
			bone_position_2 = Vector( bone_matrix_2 [ 0 ] [ 3 ], bone_matrix_2 [ 1 ] [ 3 ], bone_matrix_2 [ 2 ] [ 3 ] );

		Vector screen1, screen2;
		if ( RENDER::WorldToScreen( bone_position_1, screen1 ) && RENDER::WorldToScreen( bone_position_2, screen2 ) )
			RENDER::DrawLine( screen1.x, screen1.y, screen2.x, screen2.y, color );
	}
}

void CVisuals::DrawBulletBeams( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( !ctx::engine->IsInGame( ) || !local_player ) {
		Impacts.clear( );
		return;
	}

	if ( Impacts.size( ) > 30 )
		Impacts.pop_back( );

	for ( int i = 0; i < Impacts.size( ); i++ ) {
		auto current = Impacts.at( i );

		if ( !current.pPlayer )
			continue;

		if ( current.pPlayer->GetIsDormant( ) )
			continue;

		bool is_local_player = current.pPlayer == local_player;
		bool is_teammate = local_player->GetTeam( ) == current.pPlayer->GetTeam( ) && !is_local_player;

		if ( current.pPlayer == local_player )
			current.color = { CColor( 0, 125, 0 ) };
		else if ( current.pPlayer != local_player && !is_teammate )
			current.color = { CColor( 0, 125, 125 ) };
		else if ( current.pPlayer != local_player && is_teammate )
			current.color = { CColor( 255, 0, 0 ) };

		sdk::BeamInfo_t beamInfo;
		beamInfo.m_nType = sdk::TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = Vars::options.beam_time;
		beamInfo.m_flWidth = 2.0f;
		beamInfo.m_flEndWidth = 2.0f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current.color.RGBA [ 0 ];
		beamInfo.m_flGreen = current.color.RGBA [ 1 ];
		beamInfo.m_flBlue = current.color.RGBA [ 2 ];
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = sdk::FBEAM_ONLYNOISEONCE | sdk::FBEAM_NOTILE | sdk::FBEAM_HALOBEAM;

		beamInfo.m_vecStart = current.pPlayer->GetVecOrigin( ) + current.pPlayer->GetViewOffset( );
		beamInfo.m_vecEnd = current.vecImpactPos;

		auto beam = ctx::view_render_beams->CreateBeamPoints( beamInfo );
		if ( beam )
			ctx::view_render_beams->DrawBeam( beam );

		Impacts.erase( Impacts.begin( ) + i );
	}
}

void CVisuals::DrawCrosshair( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	auto weapon = ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) );

	if ( !weapon )
		return;

	//--- Screen Positioning ---//
	static int screen_x;
	static int screen_y;
	if ( screen_x == 0 ) {
		ctx::engine->GetScreenSize( screen_x, screen_y );
		screen_x /= 2; screen_y /= 2;
	}

	//--- Calculating Recoil ---//
	Vector view_angles;
	ctx::engine->GetViewAngles( view_angles );
	view_angles += *local_player->GetPunchAngles( ) * 2.f;
	Vector factor_vec;
	MATH::AngleVectors( view_angles, &factor_vec );
	factor_vec = factor_vec * 10000.f;
	Vector start = local_player->GetVecOrigin( ) + local_player->GetViewOffset( );
	Vector end = start + factor_vec, recoil_to_screen;

	//--- Rendering Crosshairs ---//
	switch ( Vars::options.xhair_type % 3 ) {
		case 0:
			break;
		case 1:
			RENDER::DrawF( screen_x, screen_y, FONTS::visuals_xhair_font, true, true, WHITE, "+" );
			break;
		case 2:
			if ( RENDER::WorldToScreen( end, recoil_to_screen ) && local_player->GetHealth( ) > 0 )
				RENDER::DrawF( recoil_to_screen.x, recoil_to_screen.y, FONTS::visuals_xhair_font, true, true, WHITE, "+" );
			break;
	}
}

void CVisuals::DrawDamageIndicator( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( !local_player->GetHealth( ) > 0 )
		return;


	float CurrentTime = local_player->GetTickBase( ) * ctx::globals->interval_per_tick;

	for ( int i = 0; i < dmg_indicator.size( ); i++ ) {
		if ( dmg_indicator [ i ].earse_time < CurrentTime ) {
			dmg_indicator.erase( dmg_indicator.begin( ) + i );
			continue;
		}

		if ( !dmg_indicator [ i ].initializes ) {
			dmg_indicator [ i ].Position = dmg_indicator [ i ].player->GetBonePosition( 6 );
			dmg_indicator [ i ].initializes = true;
		}

		if ( CurrentTime - dmg_indicator [ i ].last_update > 0.0001f ) {
			dmg_indicator [ i ].Position.z -= ( 0.1f * ( CurrentTime - dmg_indicator [ i ].earse_time ) );
			dmg_indicator [ i ].last_update = CurrentTime;

			Vector ScreenPosition;

			if ( RENDER::WorldToScreen( dmg_indicator [ i ].Position, ScreenPosition ) ) {
				RENDER::DrawF( ScreenPosition.x, ScreenPosition.y, FONTS::menu_window_font, true, true, CColor( 255, 255, 255, 255 ), std::to_string( dmg_indicator [ i ].dmg ).c_str( ) );
			}
		}
	}
}

void CVisuals::DrawIndicator( ) {
	//--- LBY Indication Stuff ---//
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	if ( menu_hide )
		return;

	float breaking_lby_fraction = fabs( MATH::NormalizeYaw( GLOBAL::real_angles.y - local_player->GetLowerBodyYaw( ) ) ) / 180.f;
	float lby_delta = abs( MATH::NormalizeYaw( GLOBAL::real_angles.y - local_player->GetLowerBodyYaw( ) ) );

	int screen_width, screen_height;
	ctx::engine->GetScreenSize( screen_width, screen_height );

	int center_width = screen_width / 2;
	int center_height = screen_height / 2;

	RENDER::DrawF( 10, screen_height - 88, FONTS::visuals_lby_font, false, false, CColor( ( 1.f - breaking_lby_fraction ) * 255.f, breaking_lby_fraction * 255.f, 0 ), "LBY" );

	float dmg;
	if ( autowall->CanWallbang( dmg ) ) {
		RENDER::DrawLine( center_width, center_height + 1, center_width, center_height - 1, CColor( 0, 255, 0, 255 ) );
		RENDER::DrawLine( center_width + 1, center_height, center_width - 1, center_height, CColor( 0, 255, 0, 255 ) );
	}
	else {
		RENDER::DrawLine( center_width, center_height + 1, center_width, center_height - 1, CColor( 255, 0, 0, 255 ) );
		RENDER::DrawLine( center_width + 1, center_height, center_width - 1, center_height, CColor( 255, 0, 0, 255 ) );
	}

	if ( !Vars::options.aa_bool )
		return;

	if ( !Vars::options.aa_autodir ) {
		if ( Vars::options.aa_type == 2 || Vars::options.aa_type == 4 ) {
			if ( in_tp )
				return;

			if ( Vars::options.flip_side == 0 ) { //RIGHT
				RENDER::DrawWF( ( screen_width / 2 ) + 31, screen_height / 2 - 25, FONTS::visuals_side_font, CColor( 0, 125, 255, 255 ), L"\u25B6" ); //RIGHT
				RENDER::DrawWF( ( screen_width / 2 ) - 50, screen_height / 2 - 25, FONTS::visuals_side_font, CColor( 255, 255, 255, 255 ), L"\u25C0" ); //LEFT
			}
			if ( Vars::options.flip_side == 1 ) { //LEFT
				RENDER::DrawWF( ( screen_width / 2 ) - 50, screen_height / 2 - 25, FONTS::visuals_side_font, CColor( 0, 125, 255, 255 ), L"\u25C0" ); //LEFT
				RENDER::DrawWF( ( screen_width / 2 ) + 31, screen_height / 2 - 25, FONTS::visuals_side_font, CColor( 255, 255, 255, 255 ), L"\u25B6" ); //RIGHT
			}
		}
	}
}

void CVisuals::DrawLogs( ) {

	for ( size_t i = 0; i < events.size( ); i++ ) {
		float time = ctx::globals->curtime - events [ i ].time;
		if ( time > 6.f )
			events [ i ].color.RGBA [ 3 ] *= 0.98f;

		CColor color = CColor(
			Vars::options.menu_col.RGBA [ 0 ],
			Vars::options.menu_col.RGBA [ 1 ],
			Vars::options.menu_col.RGBA [ 2 ],
			events [ i ].color.RGBA [ 3 ] );

		RENDER::DrawF( 5, 5 + i * 17, FONTS::menu_window_font, false, false, color, "[gamesneeze]" );
		RENDER::DrawF( 70, 5 + i * 17, FONTS::menu_window_font, false, false, events [ i ].color, events [ i ].text.c_str( ) );
	}
}

void CVisuals::DrawDirection( const Vector& origin ) //monarch is the NIGGA
{
	constexpr float radius = 360.0f;
	int width, height;
	ctx::engine->GetScreenSize( width, height );

	Vector vRealAngles;
	ctx::engine->GetViewAngles( vRealAngles );

	Vector vForward, vRight, vUp( 0.0f, 0.0f, 1.0f );

	MATH::AngleVectors( vRealAngles, &vForward );

	vForward.z = 0.0f;
	MATH::NormalizeAngle( vForward );
	CrossProduct( vUp, vForward, vRight );

	float flFront = VecDotProduct( origin, vForward );
	float flSide = VecDotProduct( origin, vRight );
	float flXPosition = radius * -flSide;
	float flYPosition = radius * -flFront;

	float rotation = ctx::globals->user_cmd->viewangles.y + 180;

	rotation = atan2( flXPosition, flYPosition ) + M_PI;
	rotation *= 180.0f / M_PI;

	float flYawRadians = -( rotation ) * M_PI / 180.0f;
	float flCosYaw = cos( flYawRadians );
	float flSinYaw = sin( flYawRadians );

	flXPosition = ( int ) ( ( width / 2.0f ) + ( radius * flSinYaw ) );
	flYPosition = ( int ) ( ( height / 2.0f ) - ( radius * flCosYaw ) );

	RENDER::DrawFilledCircle( flXPosition, flYPosition, 10, 50, CColor( 255, 0, 255, 120 ) );
}

void CVisuals::ModulateWorld( ) //credits to my nigga monarch
{
	static bool nightmode_performed = false, nightmode_lastsetting;

	if ( !ctx::engine->IsConnected( ) || !ctx::engine->IsInGame( ) ) {
		if ( nightmode_performed )
			nightmode_performed = false;
		return;
	}

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( !local_player ) {
		if ( nightmode_performed )
			nightmode_performed = false;
		return;
	}

	if ( nightmode_lastsetting != Vars::options.night_bool ) {
		nightmode_lastsetting = Vars::options.night_bool;
		nightmode_performed = false;
	}

	if ( !nightmode_performed ) {
		static sdk::ConVar* r_DrawSpecificStaticProp = ctx::convar->FindVar( "r_DrawSpecificStaticProp" );
		r_DrawSpecificStaticProp->nFlags &= ~FCVAR_CHEAT;

		static sdk::ConVar* sv_skyname = ctx::convar->FindVar( "sv_skyname" );
		sv_skyname->nFlags &= ~FCVAR_CHEAT;

		for ( sdk::MaterialHandle_t i = ctx::material_system->FirstMaterial( ); i != ctx::material_system->InvalidMaterial( ); i = ctx::material_system->NextMaterial( i ) ) {
			sdk::IMaterial *pMaterial = ctx::material_system->GetMaterial( i );

			if ( !pMaterial )
				continue;

			if ( strstr( pMaterial->GetTextureGroupName( ), "World" ) ) {
				if ( Vars::options.night_bool )
					pMaterial->ColorModulate( 0.6, 0.6, 0.6 );
				else
					pMaterial->ColorModulate( 1, 1, 1 );

				if ( Vars::options.night_bool ) {
					sv_skyname->SetValue( "sky_csgo_night02" );
					pMaterial->SetMaterialVarFlag( sdk::MATERIAL_VAR_TRANSLUCENT, false );
					pMaterial->ColorModulate( 0.05, 0.05, 0.05 );
				}
				else {
					sv_skyname->SetValue( "vertigoblue_hdr" );
					pMaterial->ColorModulate( 1.00, 1.00, 1.00 );
				}
			}
			else if ( strstr( pMaterial->GetTextureGroupName( ), "StaticProp" ) ) {
				if ( Vars::options.night_bool )
					pMaterial->ColorModulate( 0.28, 0.28, 0.28 );
				else
					pMaterial->ColorModulate( 1, 1, 1 );
			}
		}
		nightmode_performed = true;
	}
}

void CVisuals::DrawBacktrack( ) {

	auto local = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	sdk::player_info_t pinfo;

	for ( int i = 0; i < ctx::client_ent_list->GetHighestEntityIndex( ); i++ ) {
		auto entity = ctx::client_ent_list->GetClientEntity( i );
		if ( ctx::engine->GetPlayerInfo( i, &pinfo ) && entity->GetHealth( ) > 0 ) {
			if ( local->GetHealth( ) > 0 ) {
				for ( int t = 0; t < 12; ++t ) {
					Vector screenbacktrack [ 64 ] [ 12 ];

					if ( headPositions [ i ] [ t ].simtime && headPositions [ i ] [ t ].simtime + 1 > local->GetSimTime( ) ) {
						if ( RENDER::WorldToScreen( headPositions [ i ] [ t ].hitboxPos, screenbacktrack [ i ] [ t ] ) ) {
							RENDER::DrawFilledRect( screenbacktrack [ i ] [ t ].x, screenbacktrack [ i ] [ t ].y, screenbacktrack [ i ] [ t ].x + 2, screenbacktrack [ i ] [ t ].y + 2, CColor( 255, 255, 255, 255 ) );
						}
					}
				}
			}
			else {
				memset( &headPositions [ 0 ] [ 0 ], 0, sizeof( headPositions ) );
			}
		}
	}

}

void CVisuals::DrawHitmarker( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	int center_width;
	int center_height;
	ctx::engine->GetScreenSize( center_width, center_height );
	center_width / 2, center_height / 2;

	if ( hitmarker_time > 0 ) {
		auto line_size = 10;

		RENDER::DrawLine( center_width - line_size, center_height - line_size, center_width - ( line_size / 2 ), center_height - ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width - line_size, center_height + line_size, center_width - ( line_size / 2 ), center_height + ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width + line_size, center_height + line_size, center_width + ( line_size / 2 ), center_height + ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		RENDER::DrawLine( center_width + line_size, center_height - line_size, center_width + ( line_size / 2 ), center_height - ( line_size / 2 ), CColor( 255, 255, 255, hitmarker_time ) );
		hitmarker_time -= 5;
	}
}

void CVisuals::DrawAntiAimSides( ) {
	int screen_x;
	int screen_y;
	ctx::engine->GetScreenSize( screen_x, screen_y );
	screen_x /= 2; screen_y /= 2;

	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );
	if ( !local_player )
		return;

	if ( local_player->GetHealth( ) <= 0 )
		return;

	if ( !Vars::options.aa_type == 2 || !Vars::options.aa_type == 4 )
		return;

	if ( Vars::options.aa_side == 0 )
		RENDER::DrawWF( screen_x - 25, screen_y, FONTS::visuals_side_font, CColor( 0, 255, 0, 255 ), L"\u25c4" );
	if ( Vars::options.aa_side == 1 )
		RENDER::DrawWF( screen_x + 25, screen_y, FONTS::visuals_side_font, CColor( 0, 255, 0, 255 ), L"\u25ba" );
}

void CVisuals::DrawBorderLines( ) {
	auto local_player = ctx::client_ent_list->GetClientEntity( ctx::engine->GetLocalPlayer( ) );

	if ( !local_player )
		return;

	auto weapon = ctx::client_ent_list->GetClientEntity( local_player->GetActiveWeaponIndex( ) );

	if ( !weapon )
		return;

	//--- Screen Positioning ---//
	int screen_x;
	int screen_y;
	int center_x;
	int center_y;
	ctx::engine->GetScreenSize( screen_x, screen_y );
	ctx::engine->GetScreenSize( center_x, center_y );
	center_x /= 2; center_y /= 2;

	//--- Rendering Scope Lines ---//
	if ( local_player->GetIsScoped( ) ) {
		RENDER::DrawLine( 0, center_y, screen_x, center_y, CColor( 0, 0, 0, 255 ) );
		RENDER::DrawLine( center_x, 0, center_x, screen_y, CColor( 0, 0, 0, 255 ) );
	}
}

void CVisuals::DrawBarrel( sdk::CBaseEntity* entity ) {
	//uh fuck idk yet
}

void CVisuals::DrawBomb( sdk::CBaseEntity* entity ) {
	//:thinking:
}

CVisuals* visuals = new CVisuals( );