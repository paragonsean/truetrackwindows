#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\updown.h"
#include "..\include\subs.h"

/***********************************************************************
*                           UP_DOWN_CLASS                              *
***********************************************************************/
UP_DOWN_CLASS::UP_DOWN_CLASS( HWND dialog_box, INT id )
{
parent = dialog_box;
w      = GetDlgItem( dialog_box, id );
}

/***********************************************************************
*                          ~UP_DOWN_CLASS                              *
***********************************************************************/
UP_DOWN_CLASS::~UP_DOWN_CLASS( void )
{
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN UP_DOWN_CLASS::init( HWND dialog_box, INT id )
{
parent = dialog_box;
w      = GetDlgItem( dialog_box, id );
if ( w )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              GETVALUE                                *
***********************************************************************/
short UP_DOWN_CLASS::getvalue( void )
{
LRESULT x;

x = SendMessage( w, UDM_GETPOS, 0, 0L );

return (short) LOWORD(x);
}

/***********************************************************************
*                              SETBUDDY                                *
***********************************************************************/
BOOLEAN UP_DOWN_CLASS::setbuddy( INT buddy_id )
{
HWND    bw;

if ( w )
    {
    bw = GetDlgItem( parent, buddy_id );
    if ( bw )
        {
        SendMessage( w, UDM_SETBUDDY, (WPARAM) bw, 0L );
        return TRUE;
        }
    }
return FALSE;
}

/***********************************************************************
*                              SETDECIMAL                              *
***********************************************************************/
void    UP_DOWN_CLASS::setdecimal( void )
{
SendMessage( w, UDM_SETBASE, (WPARAM) 10, 0L );
}

/***********************************************************************
*                                SETHEX                                *
***********************************************************************/
void    UP_DOWN_CLASS::sethex( void )
{
SendMessage( w, UDM_SETBASE, (WPARAM) 16, 0L );
}

/***********************************************************************
*                              SETINCREMENT                            *
***********************************************************************/
BOOLEAN UP_DOWN_CLASS::setincrement( short increment )
{
UDACCEL uda;

uda.nInc = increment;
uda.nSec = 0;

if ( SendMessage( w, UDM_SETACCEL, (WPARAM) 1, (LPARAM) &uda ) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                               SETRANGE                               *
***********************************************************************/
void UP_DOWN_CLASS::setrange( short min_value, short max_value )
{
SendMessage( w, UDM_SETRANGE, 0, (LPARAM) MAKELONG(max_value, min_value) );
}

/***********************************************************************
*                               SETVALUE                               *
*                      Returns the previous value.                     *
***********************************************************************/
short UP_DOWN_CLASS::setvalue( short new_value )
{
return SendMessage( w, UDM_SETPOS, 0, (LPARAM) MAKELONG(new_value, 0) );
}

