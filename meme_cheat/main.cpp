#include "includes.h"

#include "utilities\interfaces.h"
#include "hooks\hooks.h"
#include "utilities\offsets.h"
#include "features\event listeners\EventListener.h"

#include "sdk\IEngine.h"

#include "gui\newgui\menu.hpp"


#include <chrono>
#include <thread>

bool using_fake_angles[65];

bool in_tp;
bool fake_walk;

int resolve_type[65];

int shots_fired[65];
int shots_hit[65];
int shots_missed[65];

float tick_to_back[65];
float lby_to_back[65];
bool backtrack_tick[65];

float lby_delta;
float update_time[65];

int hitmarker_time;

bool menu_hide;

int oldest_tick[65];
float compensate[65][12];
Vector backtrack_hitbox[65][20][12];
float backtrack_simtime[65][12];

DWORD GetFuncSize(DWORD* Function, DWORD* StubFunction)
{
	DWORD dwFunctionSize = 0, dwOldProtect;
	DWORD *fnA = NULL, *fnB = NULL;

	fnA = (DWORD *)Function;
	fnB = (DWORD *)StubFunction;
	dwFunctionSize = (fnB - fnA);
	VirtualProtect(fnA, dwFunctionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect); // Need to modify our privileges to the memory
	return dwFunctionSize;
}

unsigned long __stdcall Start(void* reserved)
{
	AllocConsole( );
	auto hwnd = GetConsoleWindow( );
	auto hMenu = GetSystemMenu( hwnd, FALSE );
	if ( hMenu )
		DeleteMenu( hMenu, SC_CLOSE, MF_BYCOMMAND );

	SetConsoleTitle( "Console:" );
	freopen( "CONIN$", "r", stdin );
	freopen( "CONOUT$", "w", stdout );
	


	ctx::InitInterfaces();
	OFFSETS::InitOffsets();

	UTILS::INPUT::input_handler.Init();
	FONTS::InitFonts();
//	MENU::InitColors();
	gui::initialize( );

	HOOKS::InitHooks();
	HOOKS::InitNetvarHooks();
	CGameEvents::InitializeEventListeners();

	while ( !GetAsyncKeyState( VK_DELETE ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

	HOOKS::KillHooks( );
	gui::shutdown( );
	UTILS::INPUT::input_handler.Kill();
	CGameEvents::RemoveListeners();

	std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	fclose( ( FILE* ) stdin );
	fclose( ( FILE* ) stdout );
	FreeConsole( );

	FreeLibraryAndExitThread( reinterpret_cast< HMODULE >( reserved ), 0 );
	return 0ul;
}

__declspec(noinline) void start_end()
{
	return;
}

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, Instance, NULL, NULL);
	}
	return true;
}

