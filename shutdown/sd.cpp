#include <windows.h>

#include "..\include\visiparm.h"
#define _MAIN_
#include "..\include\events.h"

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
int WINAPI WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{

client_dde_startup();
poke_data( DDE_EXIT_TOPIC,  ItemList[NEW_MACH_STATE_INDEX].name, UNKNOWN );
client_dde_shutdown();

return 0;
}

