#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"


/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
FILE_CLASS f;
f.open_for_write( "bozo.boz" );
f.writeline( "hi you guys" );
f.close();

return( 0 );
}
