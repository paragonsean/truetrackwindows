#include <windows.h>

#include "..\include\visiparm.h"

static HCURSOR OldCursor = 0;
static int32   RefCount  = 0;

/***********************************************************************
*                           HOURGLASS_CURSOR                           *
***********************************************************************/
void hourglass_cursor( void )
{
if ( RefCount == 0 )
    OldCursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

RefCount++;
}

/***********************************************************************
*                            RESTORE_CURSOR                            *
***********************************************************************/
void restore_cursor( void )
{
if ( OldCursor && RefCount )
    {
    RefCount--;
    if ( RefCount == 0 )
        SetCursor( OldCursor );
    }
}
