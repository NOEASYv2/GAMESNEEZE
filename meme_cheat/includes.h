#pragma once

//this has all common includes that most (every?) cpp file should include

#include <Windows.h>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <assert.h>
#include <Psapi.h>
#include <thread>
#include <chrono>
#include "gui\newgui\imgui\imgui.h"
#include "gui\newgui\imgui\imgui_impl_dx9.h"

#define DEV_MODE 1

#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

enum MoveType {
	MOVETYPE_NONE = 0,            /**< never moves */
	MOVETYPE_ISOMETRIC,            /**< For players */
	MOVETYPE_WALK,                /**< Player only - moving on the ground */
	MOVETYPE_STEP,                /**< gravity, special edge handling -- monsters use this */
	MOVETYPE_FLY,                /**< No gravity, but still collides with stuff */
	MOVETYPE_FLYGRAVITY,        /**< flies through the air + is affected by gravity */
	MOVETYPE_VPHYSICS,            /**< uses VPHYSICS for simulation */
	MOVETYPE_PUSH,                /**< no clip to world, push and crush */
	MOVETYPE_NOCLIP,            /**< No gravity, no collisions, still do velocity/avelocity */
	MOVETYPE_LADDER,            /**< Used by players only when going onto a ladder */
	MOVETYPE_OBSERVER,            /**< Observer movement, depends on player's observer mode */
	MOVETYPE_CUSTOM,            /**< Allows the entity to describe its own physics */
};


#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)   
#define IN_SPEED					(1 << 17)	
#define IN_WALK						(1 << 18)	
#define IN_ZOOM						(1 << 19)	
#define IN_WEAPON1					(1 << 20)	
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)	
#define	IN_ATTACK3					(1 << 25)

#define	FL_ONGROUND					(1 << 0)	
#define FL_DUCKING					(1 << 1)	
#define	FL_WATERJUMP				(1 << 2)	
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)	
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)	
#define FL_FAKECLIENT				(1 << 8)	
#define	FL_INWATER					(1 << 9)										
#define	FL_FLY						(1 << 10)	
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)	
#define	FL_PARTIALGROUND			(1 << 17)	
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19) 
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)	
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)	
#define FL_OBJECT					(1 << 25) 
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)	
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30) 

#define PLAYER_FLAG_BITS			10
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

#define HITGROUP_GENERIC	  0
#define HITGROUP_HEAD         1
#define HITGROUP_CHEST        2
#define HITGROUP_STOMACH      3
#define HITGROUP_LEFTARM      4    
#define HITGROUP_RIGHTARM     5
#define HITGROUP_LEFTLEG      6
#define HITGROUP_RIGHTLEG     7
#define HITGROUP_GEAR         10
#define SURF_HITBOX           0x8000

#define MAX_STUDIOBONES 128
#define MAXSTUDIOSKINS 32

#define	SURF_SKY		0x0004	
#define BONE_USED_BY_HITBOX	0x00000100
#define BONE_USED_BY_ANYTHING   0x0007FF00

// hhh macros
#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / ctx::globals->interval_per_tick ) )
#define TICKS_TO_TIME( t )		( ctx::globals->interval_per_tick *( t ) )


#include "utilities\vmt.h"
#include "utilities\vector3D.h"
#include "utilities\vector2D.h"
#include "utilities\vmatrix.h"
#include "utilities\quaternion.h"
#include "utilities\math.h"
#include "utilities\general_utils.h"
#include "utilities\color.h"

#include "utilities\input_handler.h"
#include "utilities\variables.h"
#include "utilities\playerinclude.h"


// for fsn
enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};