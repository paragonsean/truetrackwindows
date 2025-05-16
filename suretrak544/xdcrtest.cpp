#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\bitclass.h"
#include "..\include\subs.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\ftclass.h"

#include "resource.h"

#include "extern.h"

const static int32 NofPoints = 500;

const static UINT CHANNEL_A_FRAME = 1;
const static UINT CHANNEL_B_FRAME = 2;

BIT_CLASS Channel_A_Bits( NofPoints );
BIT_CLASS Channel_B_Bits( NofPoints );

void send_command_to_board( BOARD_DATA command );

/***********************************************************************
*                        SCREEN_TO_CLIENT_RECT                         *
***********************************************************************/
BOOLEAN screen_to_client_rect( HWND w, RECT & r )
{

BOOL  status;
POINT p;

p.x = r.left;
p.y = r.top;

status = ScreenToClient( w, &p );

r.left = p.x;
r.top  = p.y;

if ( status )
    {
    p.x = r.right;
    p.y = r.bottom;

    status = ScreenToClient( w, &p );

    r.right  = p.x;
    r.bottom = p.y;
    }

return status;
}


/***********************************************************************
*                            PLOT_ONE_CURVE                            *
***********************************************************************/
static void plot_one_curve( HDC dc, BIT_CLASS & bits, UINT mychannel )
{
HWND  w;
RECT  r;
int   i;
int   high_y;
int   low_y;
int   y;
int   x;
int   last_x;
int   last_y;
int   delta_x;

last_x = 0;
last_y = 0;

w = GetDlgItem( TransducerTestWindow, CHANNEL_FRAME );

if ( GetWindowRect( w, &r ) )
    {
    screen_to_client_rect( TransducerTestWindow, r );

    y = r.top + (r.bottom - r.top)/2;

    if ( mychannel == CHANNEL_A_FRAME )
        r.bottom = y;
    else
        r.top = y;

    delta_x = r.right  - r.left;

    y = (r.bottom - r.top) / 20;
    low_y = r.bottom - y;
    high_y = r.top + y;

    for ( i=0; i<NofPoints; i++ )
        {
        x = i * delta_x;
        x /= NofPoints;
        x += r.left;

        if ( bits[i] )
            y = high_y;
        else
            y = low_y;

        if ( i == 0 )
            {
            MoveToEx( dc, x, y, 0 );
            }
        else
            {
            if ( x != last_x )
                LineTo( dc, x, last_y );

            if ( y != last_y )
                LineTo( dc, x, y );
            }

        last_x = x;
        last_y = y;
        }

    }

}

/***********************************************************************
*                              PAINT_ME                                *
***********************************************************************/
static void paint_me( HWND w )
{

PAINTSTRUCT  ps;
HPEN   mypen;
HPEN   oldpen;

const COLORREF blackcolor = RGB( 0, 0, 0 );

if ( !GetUpdateRect(w, NULL, FALSE) )
    return;

BeginPaint( w, &ps );

/*
------------
Create a pen
------------ */
oldpen = 0;
mypen  = CreatePen( PS_SOLID, 1, blackcolor );

if ( mypen )
    oldpen = (HPEN) SelectObject( ps.hdc, mypen );

/*
---------------
Plot the curves
--------------- */
plot_one_curve( ps.hdc, Channel_A_Bits, CHANNEL_A_FRAME );
plot_one_curve( ps.hdc, Channel_B_Bits, CHANNEL_B_FRAME );

/*
--------------
Delete the pen
-------------- */
if ( oldpen )
    SelectObject( ps.hdc, oldpen );

if ( mypen )
    DeleteObject( mypen );

EndPaint( w, &ps );
}

/***********************************************************************
*                     TRANSDUCER_TEST_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK transducer_test_dialog_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int   id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        if ( !(SureTrakInputWord & MANUAL_MODE_BIT) )
            resource_message_box(MainInstance, PRESS_OK_WHEN_READY_STRING, NOT_MANUAL_MODE_STRING, MB_OK | MB_SYSTEMMODAL );
        TransducerTestWindow = w;
        return TRUE;

    case WM_PAINT:
        paint_me( w );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case NEW_SAMPLE_BUTTON:
                send_command_to_board( TRANSDUCER_TEST_REQUEST );
                return TRUE;

            case IDCANCEL:
                TransducerTestWindow = 0;
                EndDialog( w, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        TransducerTestWindow = 0;
        EndDialog( w, 0 );
        return TRUE;
    }

return FALSE;
}
