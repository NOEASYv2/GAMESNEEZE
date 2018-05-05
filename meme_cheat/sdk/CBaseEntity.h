#pragma once

#include "../../utilities/offsets.h"
#include "../../utilities/interfaces.h"
#include "../../sdk/ModelInfo.h"
#include "../../utilities/qangle.h"
#include "../../utilities/Vector4D.h"

#include "AnimLayer.h"
#include "RecvData.h"

namespace sdk {
	class CBaseAnimState;
	class Collideable;

	struct studiohdr_t;
	struct model_t;


	enum WEAPONCLASS {
		WEPCLASS_INVALID,
		WEPCLASS_RIFLE,
		WEPCLASS_PISTOL,
		WEPCLASS_SHOTGUN,
		WEPCLASS_SNIPER,
		WEPCLASS_SMG,
		WEPCLASS_MACHINEGUN,
		WEPCLASS_KNIFE,
	};

	enum ItemDefinitionIndex : int {
		WEAPON_DEAGLE = 1,
		WEAPON_ELITE = 2,
		WEAPON_FIVESEVEN = 3,
		WEAPON_GLOCK = 4,
		WEAPON_AK47 = 7,
		WEAPON_AUG = 8,
		WEAPON_AWP = 9,
		WEAPON_FAMAS = 10,
		WEAPON_G3SG1 = 11,
		WEAPON_GALILAR = 13,
		WEAPON_M249 = 14,
		WEAPON_M4A1 = 16,
		WEAPON_MAC10 = 17,
		WEAPON_P90 = 19,
		WEAPON_UMP45 = 24,
		WEAPON_XM1014 = 25,
		WEAPON_BIZON = 26,
		WEAPON_MAG7 = 27,
		WEAPON_NEGEV = 28,
		WEAPON_SAWEDOFF = 29,
		WEAPON_TEC9 = 30,
		WEAPON_TASER = 31,
		WEAPON_HKP2000 = 32,
		WEAPON_MP7 = 33,
		WEAPON_MP9 = 34,
		WEAPON_NOVA = 35,
		WEAPON_P250 = 36,
		WEAPON_SCAR20 = 38,
		WEAPON_SG556 = 39,
		WEAPON_SSG08 = 40,
		WEAPON_KNIFE_CT = 42,
		WEAPON_FLASHBANG = 43,
		WEAPON_HEGRENADE = 44,
		WEAPON_SMOKEGRENADE = 45,
		WEAPON_MOLOTOV = 46,
		WEAPON_DECOY = 47,
		WEAPON_INCGRENADE = 48,
		WEAPON_C4 = 49,
		WEAPON_KNIFE_T = 59,
		WEAPON_M4A1_SILENCER = 60,
		WEAPON_USP_SILENCER = 61,
		WEAPON_CZ75A = 63,
		WEAPON_REVOLVER = 64,
		WEAPON_KNIFE_BAYONET = 500,
		WEAPON_KNIFE_FLIP = 505,
		WEAPON_KNIFE_GUT = 506,
		WEAPON_KNIFE_KARAMBIT = 507,
		WEAPON_KNIFE_M9_BAYONET = 508,
		WEAPON_KNIFE_TACTICAL = 509,
		WEAPON_KNIFE_FALCHION = 512,
		WEAPON_KNIFE_BOWIE = 514,
		WEAPON_KNIFE_BUTTERFLY = 515,
		WEAPON_KNIFE_PUSH = 516
	};

	class CBaseAnimating {
	public:
		std::array<float, 24>* m_flPoseParameter( ) {
			static int offset = 0;
			if ( !offset )
				offset = OFFSETS::m_flPoseParameter;
			return ( std::array<float, 24>* )( ( uintptr_t ) this + offset );
		}
		model_t* GetModel( ) {
			void* pRenderable = reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
			typedef model_t* ( __thiscall* fnGetModel )( void* );

			return VMT::VMTHookManager::GetFunction<fnGetModel>( pRenderable, 8 )( pRenderable );
		}
		void SetBoneMatrix( matrix3x4_t* boneMatrix ) {
			//Offset found in C_BaseAnimating::GetBoneTransform, string search ankle_L and a function below is the right one
			const auto model = this->GetModel( );
			if ( !model )
				return;

			matrix3x4_t* matrix = *( matrix3x4_t** ) ( ( DWORD ) this + 9880 );
			studiohdr_t *hdr = ctx::model_info->GetStudioModel( model );
			if ( !hdr )
				return;
			int size = hdr->numbones;
			if ( matrix ) {
				for ( int i = 0; i < size; i++ )
					memcpy( matrix + i, boneMatrix + i, sizeof( matrix3x4_t ) );
			}
		}
		void GetDirectBoneMatrix( matrix3x4_t* boneMatrix ) {
			const auto model = this->GetModel( );
			if ( !model )
				return;

			matrix3x4_t* matrix = *( matrix3x4_t** ) ( ( DWORD ) this + 9880 );
			studiohdr_t *hdr = ctx::model_info->GetStudioModel( model );
			if ( !hdr )
				return;
			int size = hdr->numbones;
			if ( matrix ) {
				for ( int i = 0; i < size; i++ )
					memcpy( boneMatrix + i, matrix + i, sizeof( matrix3x4_t ) );
			}
		}
	};
	class CBaseEntity {
	public:
		int GetHealth( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_iHealth );
		}
		int GetFlags( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_fFlags );
		}
		int GetMoveType( ) {
			return *reinterpret_cast< int* >( ( DWORD ) this + ( DWORD ) OFFSETS::m_iMoveType );
		}
		void SetFlags( int flags ) {
			*reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_fFlags ) = flags;
		}
		UINT* GetWeaponsUint( ) {
			return ( UINT* ) ( ( DWORD ) this + 0x00002DE8 );
		}
		HANDLE* GetWeaponsHandle( ) {
			return ( HANDLE* ) ( ( DWORD ) this + 0x00002DE8 );
		}
		void set_angles( vec3_t angles ) {
			using original_fn = void( __thiscall* )( void*, const vec3_t& );
			static original_fn set_angles_fn = ( original_fn ) ( ( DWORD ) UTILS::pattern_scan( GetModuleHandleA( "client.dll" ), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1" ) );
			set_angles_fn( this, angles );
		}
		int GetTeam( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_iTeamNum );
		}
		int GetObserverMode( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_iObserverMode );
		}
		HANDLE GetObserverModeHandle( ) {
			return *( HANDLE* ) ( ( uintptr_t ) this + OFFSETS::m_iObserverMode );
		}
		int SetObserverMode( int mode ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_iObserverMode ) = mode;
		}
		bool GetIsScoped( ) {
			return *reinterpret_cast< bool* >( uintptr_t( this ) + OFFSETS::m_bIsScoped );
		}
		Vector GetVelocity( ) {
			return *reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_vecVelocity );
		}
		void SetVelocity( Vector velocity ) {
			*reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_vecVelocity ) = velocity;
		}
		int GetMoney( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_iAccount );
		}
		Vector* GetHeadRotation( ) {
			return reinterpret_cast<Vector*>( ( DWORD ) this + 0x0128 );
		}
		float GetLowerBodyYaw( ) {
			return *reinterpret_cast< float* >( uintptr_t( this ) + OFFSETS::m_flLowerBodyYawTarget );
		}
		Vector GetVecOrigin( ) {
			return *reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_vecOrigin );
		}
		Vector& GetAbsOrigin( ) {
			typedef Vector& ( __thiscall* OriginalFn )( void* );
			return ( ( OriginalFn ) VMT::VMTHookManager::GetFunction<OriginalFn>( this, 10 ) )( this );
		}
		int GetTickBase( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_nTickBase );
		}
		bool GetIsDormant( ) {
			return *reinterpret_cast< bool* >( uintptr_t( this ) + OFFSETS::m_bDormant );
		}
		CBaseAnimState* GetAnimState( ) {
			return *reinterpret_cast< CBaseAnimState** >( uintptr_t( this ) + OFFSETS::animstate );
		}
		Collideable* GetCollideable( ) {
			return ( Collideable* ) ( ( DWORD ) this + OFFSETS::m_Collision );
		}
		const matrix3x4_t& m_rgflCoordinateFrame( ) {
			return *( matrix3x4_t* ) ( ( uintptr_t ) this + OFFSETS::m_Collision - 0x30 );
		}
		void GetRenderBounds( Vector& mins, Vector& maxs ) {
			void* pRenderable = ( void* ) ( this + 0x4 );
			typedef void( __thiscall* Fn )( void*, Vector&, Vector& );
			VMT::VMTHookManager::GetFunction<Fn>( pRenderable, 17 )( pRenderable, mins, maxs );
		}
		int GetIndex( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + 0x64 );
		}
		CAnimationLayer& GetAnimOverlay( int Index ) {
			return ( *( CAnimationLayer** ) ( ( DWORD ) this + 0x2970 ) ) [ Index ];
		}
		void SetAnimOverlay( int Index, CAnimationLayer layer ) {
			( *( CAnimationLayer** ) ( ( DWORD ) this + 0x2970 ) ) [ Index ] = layer;
		}
		int CBaseEntity::GetSequenceActivity( int sequence ) {
			const auto model = GetModel( );
			if ( !model )
				return -1;

			const auto hdr = ctx::model_info->GetStudioModel( model );
			if ( !hdr )
				return -1;

			static auto offset = ( DWORD ) UTILS::FindSignature( "client.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" );
			static auto GetSequenceActivity = reinterpret_cast< int( __fastcall* )( void*, sdk::studiohdr_t*, int ) >( offset );

			return GetSequenceActivity( this, hdr, sequence );
		}
		Vector GetEyeAngles( ) {
			return *reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_angEyeAngles );
		}
		Vector* GetEyeAnglesPtr( ) {
			return reinterpret_cast<Vector*>( ( DWORD ) this + OFFSETS::m_angEyeAngles );
		}
		vec3_t get_eye_ang( ) {
			return *reinterpret_cast< vec3_t* >( uintptr_t( this ) + OFFSETS::m_angEyeAngles );
		}

		Vector GetOrigin( ) {
			return *( Vector* ) ( this + OFFSETS::m_vecOrigin );

		}
		Vector GetEyePosition( ) {
			return *( Vector* ) ( this + OFFSETS::m_vecViewOffset ) + GetOrigin( );
		}

		QAngle* EasyEyeAngles( ) {
			return ( QAngle* ) ( ( uintptr_t ) this + OFFSETS::m_angEyeAngles );
		}
		void SetEyeAngles( Vector angles ) {
			*reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_angEyeAngles ) = angles;
		}
		float GetSimTime( ) {
			return *reinterpret_cast< float* >( uintptr_t( this ) + OFFSETS::m_flSimulationTime );
		}
		Vector GetViewOffset( ) {
			return *reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_vecViewOffset );
		}
		model_t* GetModel( ) {
			void* pRenderable = reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
			typedef model_t* ( __thiscall* fnGetModel )( void* );

			return VMT::VMTHookManager::GetFunction<fnGetModel>( pRenderable, 8 )( pRenderable );
		}
		matrix3x4_t GetBoneMatrix( int BoneID ) {
			matrix3x4_t matrix;

			auto offset = *reinterpret_cast< uintptr_t* >( uintptr_t( this ) + OFFSETS::m_dwBoneMatrix );
			if ( offset )
				matrix = *reinterpret_cast< matrix3x4_t* >( offset + 0x30 * BoneID );

			return matrix;
		}
		Vector GetBonePosition( int i ) {
			matrix3x4_t matrix [ 128 ];
			if ( this->SetupBones( matrix, 128, BONE_USED_BY_ANYTHING, GetTickCount64( ) ) ) {
				return Vector( matrix [ i ] [ 0 ] [ 3 ], matrix [ i ] [ 1 ] [ 3 ], matrix [ i ] [ 2 ] [ 3 ] );
			}
			return Vector( 0, 0, 0 );
		}
		Vector* GetPunchAngles( ) {
			return reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_aimPunchAngle );
		}
		Vector* GetPunchView( ) {
			return reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_aimPunchAngle );
		}

		Vector* AimPunchAngle( ) {
			return reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_aimPunchAngle );
		}

		Vector* ViewPunchAngle( ) {
			return reinterpret_cast< Vector* >( uintptr_t( this ) + OFFSETS::m_viewPunchAngle );
		}
		bool GetImmunity( ) {
			return *reinterpret_cast< bool* >( uintptr_t( this ) + OFFSETS::m_bGunGameImmunity );
		}
		bool SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) {
			void* pRenderable = reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
			if ( !pRenderable )
				false;

			typedef bool( __thiscall* Fn )( void*, matrix3x4_t*, int, int, float );
			return VMT::VMTHookManager::GetFunction<Fn>( pRenderable, 13 )( pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime );
		}
		void UpdateClientSideAnimation( ) {
			typedef void( __thiscall* original )( void* );
			VMT::VMTHookManager::GetFunction<original>( this, 218 )( this );
		}
		float GetSpread( ) {
			typedef float( __thiscall* original )( void* );
			return VMT::VMTHookManager::GetFunction<original>( this, 439 )( this );
		}
		float GetInaccuracy( ) {
			typedef float( __thiscall* oInaccuracy )( PVOID );
			return VMT::VMTHookManager::GetFunction<oInaccuracy>( this, 470 )( this );
		}
		void SetAbsOrigin( Vector ArgOrigin ) {
			using Fn = void( __thiscall* )( CBaseEntity*, const Vector &origin );
			static Fn func;

			if ( !func )
				func = ( Fn ) ( UTILS::FindPattern( "client.dll", ( PBYTE )"\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1\xE8\x00\x00", "xxxxxxxxxxxxx??" ) );

			func( this, ArgOrigin );
		}
		Vector& GetAbsAngles( ) {
			typedef Vector& ( __thiscall* OriginalFn )( void* );
			return ( ( OriginalFn ) VMT::VMTHookManager::GetFunction<OriginalFn>( this, 11 ) )( this );
		}
		void SetAbsAngles( Vector angles ) {
			using Fn = void( __thiscall* )( CBaseEntity*, const Vector &angles );
			static Fn func;

			if ( !func )
				func = ( Fn ) ( UTILS::FindPattern( "client.dll", ( BYTE* )"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx" ) );

			func( this, angles );
		}
		float* GetPoseParamaters( ) {
			return reinterpret_cast< float* >( uintptr_t( this ) + OFFSETS::m_flPoseParameter );
		}
		Vector GetRenderAngles( ) {
			return *( Vector* ) ( ( DWORD ) this + 0x128 );
		}
		void SetRenderAngles( Vector angles ) {
			*( Vector* ) ( ( DWORD ) this + 0x128 ) = angles;
		}
		ClientClass* GetClientClass( ) {
			void* Networkable = ( void* ) ( this + 0x8 );
			typedef ClientClass*( __thiscall* OriginalFn )( void* );
			return VMT::VMTHookManager::GetFunction<OriginalFn>( Networkable, 2 )( Networkable );
		}
		float GetNextAttack( ) {
			return *reinterpret_cast< float* >( uint32_t( this ) + OFFSETS::m_flNextAttack );
		}
		int DrawModel( int flags, uint8_t alpha ) {
			void* pRenderable = ( void* ) ( this + 0x4 );

			using fn = int( __thiscall* )( void*, int, uint8_t );
			return VMT::VMTHookManager::GetFunction< fn >( pRenderable, 9 )( pRenderable, flags, alpha );
		}
		int GetActiveWeaponIndex( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_hActiveWeapon ) & 0xFFF;
		}
		int GetArmor( ) {
			return *reinterpret_cast< int* >( uintptr_t( this ) + OFFSETS::m_ArmorValue );
		}
		bool HasHelmet( ) {
			return *reinterpret_cast< bool* >( uintptr_t( this ) + OFFSETS::m_bHasHelmet );
		}
		char* GetArmorName( ) {
			if ( GetArmor( ) > 0 ) {
				if ( HasHelmet( ) )
					return "hk";
				else
					return "k";
			}
			else
				return " ";
		}
		bool HasC4( ) {
			int iBombIndex = *( int* ) ( *( DWORD* ) ( OFFSETS::dwPlayerResource ) + OFFSETS::m_iPlayerC4 );
			if ( iBombIndex == this->GetIndex( ) )
				return true;
			else
				return false;
		}
	};
}