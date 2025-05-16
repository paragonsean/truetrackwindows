#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\ftii.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#include "resource.h"
#include "extern.h"

COLORREF Xdcr_Chan_A_Color = RGB( 255,   0,   0 );
COLORREF Xdcr_Chan_B_Color = RGB(   0, 255,   0 );
COLORREF Xdcr_BG_Color     = RGB( 128, 158, 156 );
COLORREF Xdcr_Grid_Color   = RGB( 128, 128, 128 );
COLORREF Xdcr_Axis_Color   = RGB(   0,   0,   0 );
HBRUSH   Xdcr_BG_Brush     = 0;
int      CurrentPosIndex   = 4;

/***********************************************************************
*                              HLINE                                   *
***********************************************************************/
static void hline( HDC dc, int x1, int x2, int y )
{
MoveToEx( dc, x1,  y, 0 );
LineTo(   dc, x2,  y    );
}

/***********************************************************************
*                              VLINE                                   *
***********************************************************************/
static void vline( HDC dc, int x, int y1, int y2 )
{
MoveToEx( dc, x,  y1, 0 );
LineTo(   dc, x,  y2    );
}

/***********************************************************************
*                         PAINT_XDCR_BACKGROUND                        *
***********************************************************************/
static void paint_xdcr_background( HWND w, HDC dc )
{
HBRUSH oldbrush;
RECT r;

GetClientRect(w, &r);

oldbrush = (HBRUSH) SelectObject( dc, Xdcr_BG_Brush );

FillRect( dc, &r, Xdcr_BG_Brush );

SelectObject( dc, oldbrush );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static bool paint_me( void )
{
const int Q1 = 0;
const int Q2 = 1;
const int Q3 = 2;
const int Q4 = 3;
const int nof_positions= 4;
const int nof_points   = 2*nof_positions;

RECTANGLE_CLASS r;
RECTANGLE_CLASS pr;
PAINTSTRUCT ps;
HDC         dc;
HPEN        axispen;
HPEN        apen;
HPEN        bpen;
HPEN        oldpen;
FTII_FILE_DATA fd;
int            axis_y;
int            axis_width;
int            ch;
int            cw;
int            delta_count;
int            high_y;
int            i;
int            j;
int            q;
int            low_y;
int            max_q;
int            midy;
int            min_q;
int            pi;
int            text_y;
int            tick_height;
int            x;
int            y;
POINT          p[nof_points];
unsigned       count;
unsigned       max_count;
unsigned       min_count;
double         d;
STRING_CLASS   s;


if ( !fd.np || HaveNewXdcrPart )
    {
    s = profile_ii_name( ComputerName, MachineName, PartName, ShotName );
    if ( !fd.get(s) )
        {
        BeginPaint( XdcrWindow.handle(), &ps );
        EndPaint( XdcrWindow.handle(), &ps );
        return true;
        }
    HaveNewXdcrPart = false;
    }

XdcrWindow.get_client_rect( r );
BeginPaint( XdcrWindow.handle(), &ps );
dc = ps.hdc;

pi = CurrentPosIndex;
if ( pi < 2 )
    {
    pi = 2;
    }

if ( pi >= (fd.np-nof_positions) )
    pi = fd.np-nof_positions;

CurrentPosIndex = pi;

/*
------------------------------------------------------
Get the min and max velcount values for the two points
------------------------------------------------------ */
min_q = 0;
min_count = fd.position_sample[pi].velcount[0];

for ( q=1; q<X4_COUNTS_PER_MARK; q++ )
    {
    if ( fd.position_sample[pi].velcount[q] < min_count )
        {
        min_q = q;
        min_count = fd.position_sample[pi].velcount[q];
        }
    }

max_q = min_q - 1;
if ( max_q < 0 )
    max_q = X4_COUNTS_PER_MARK - 1;

max_count   = fd.position_sample[pi+nof_positions-1].velcount[max_q];
delta_count = (int)(max_count - min_count);
if ( delta_count == 0 )
    delta_count = 1;

/*
---------------------------------
Calculate the positions of things
--------------------------------- */
cw = average_character_width( dc );
ch = character_height( dc );

tick_height = ch/4;
axis_y = r.bottom - ch - 2*tick_height;
text_y = axis_y + tick_height;

pr.left    = r.left + 3 * cw;
pr.right   = r.right - 3*cw;
pr.top     = r.top;
pr.bottom  = axis_y - 1;
midy       = (pr.top + pr.bottom) / 2;
axis_width = pr.width();

axispen = CreatePen( PS_SOLID, 0, Xdcr_Axis_Color   );
apen    = CreatePen( PS_SOLID, 0, Xdcr_Chan_A_Color );
bpen    = CreatePen( PS_SOLID, 0, Xdcr_Chan_B_Color );

/*
-------------
Draw the axis
------------- */
oldpen = (HPEN) SelectObject( dc, axispen );
hline( dc, pr.left, pr.right, axis_y );

/*
--------------
Draw the ticks
-------------- */
vline( dc, pr.right,             axis_y, axis_y+tick_height );
vline( dc, pr.left,              axis_y, axis_y+tick_height );
vline( dc, (pr.left+pr.right)/2, axis_y, axis_y+tick_height );

/*
------------------
Draw the A channel
------------------ */
SelectObject( dc, apen );

high_y = pr.top + tick_height;
low_y  = midy   - tick_height;

if ( min_q == Q1 || min_q == Q4 )
    {
    /*
    ------------------------------------------------------
    The next point is Q1, meaning that I am low until then
    ------------------------------------------------------ */
    y = low_y;
    j = 0;
    for ( i=0; i<nof_positions; i++ )
        {
        count = fd.position_sample[pi+i].velcount[Q1];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = high_y;
        j++;

        count = fd.position_sample[pi+i].velcount[Q3];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = low_y;
        j++;
        }
    }
else
    {
    y = high_y;
    j = 0;
    for ( i=0; i<nof_positions; i++ )
        {
        count = fd.position_sample[pi+i].velcount[Q3];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = low_y;
        j++;

        count = fd.position_sample[pi+i].velcount[Q1];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = high_y;
        j++;
        }
    }

x = pr.left;
MoveToEx( dc, x, y, 0 );
for ( q=0; q<nof_points; q++ )
    {
    if ( p[q].x > x )
        {
        x = p[q].x;
        LineTo( dc, x, y );
        }
    y = p[q].y;
    LineTo( dc, x, y );
    }

if ( x < pr.right )
    LineTo( dc, pr.right, y );

/*
------------------
Draw the B channel
------------------ */
SelectObject( dc, bpen );

high_y = midy      + tick_height;
low_y  = pr.bottom - tick_height;

if ( min_q == Q1 || min_q == Q2 )
    {
    /*
    ------------------------------------------------------
    The next point is Q2, meaning that I am low until then
    ------------------------------------------------------ */
    y = low_y;
    j = 0;
    for ( i=0; i<nof_positions; i++ )
        {
        count = fd.position_sample[pi+i].velcount[Q2];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = high_y;
        j++;

        count = fd.position_sample[pi+i].velcount[Q4];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = low_y;
        j++;
        }
    }
else
    {
    y = high_y;
    j = 0;
    for ( i=0; i<nof_positions; i++ )
        {
        count = fd.position_sample[pi+i].velcount[Q4];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = low_y;
        j++;

        count = fd.position_sample[pi+i].velcount[Q2];
        p[j].x = pr.left + (count - min_count) * axis_width / delta_count;
        p[j].y = high_y;
        j++;
        }
    }

x = pr.left;
MoveToEx( dc, x, y, 0 );
for ( q=0; q<nof_points; q++ )
    {
    if ( p[q].x > x )
        {
        x = p[q].x;
        LineTo( dc, x, y );
        }
    y = p[q].y;
    LineTo( dc, x, y );
    }

if ( x < pr.right )
    LineTo( dc, pr.right, y );

SetTextAlign( dc, TA_TOP | TA_CENTER );
SetTextColor( dc, Xdcr_Axis_Color );
SetBkMode(    dc, TRANSPARENT );

j = 0;
for ( i=0; i<nof_positions; i++ )
    {
    d = CurrentPart.dist_from_x4( pi*X4_COUNTS_PER_MARK );
    d = round( d, 0.01 );
    s = ascii_double( d );
    x = (p[j].x + p[j+1].x)/2;
    TextOut( dc, x, text_y, s.text(), s.len() );
    j += 2;
    pi++;
    }

SelectObject( dc, oldpen );
DeleteObject( axispen );
DeleteObject( apen );
DeleteObject( bpen );

EndPaint( XdcrWindow.handle(), &ps );
return true;
}

/***********************************************************************
*                               XDCR_PROC                              *
***********************************************************************/
LRESULT CALLBACK xdcr_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int    id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        XdcrWindow = hWnd;
        XdcrWindow.post( WM_DBINIT );
        Xdcr_BG_Brush = (HBRUSH) CreateSolidBrush( Xdcr_BG_Color );
        break;

    case WM_DESTROY:
        if ( Xdcr_BG_Brush )
            {
            DeleteObject( Xdcr_BG_Brush );
            Xdcr_BG_Brush = 0;
            }
        break;

    case WM_PAINT:
        if ( paint_me() )
            return 0;
        break;

    case WM_NEW_DATA:
        if ( wParam == 0 && lParam != 0 )
            {
            /*
            ---------------------------
            This is a direction message
            --------------------------- */
            id = (int) lParam;
            CurrentPosIndex += id;
            }
        else
            {
            CurrentPosIndex = wParam/4;
            }
        XdcrWindow.refresh();
        return 0;

    case WM_ERASEBKGND:
        paint_xdcr_background( hWnd, (HDC) wParam );
        return 1;  /* 1 tells windows I have erased the window */

    case WM_DBINIT:
        XdcrWindow.hide();
        break;

    case WM_SETFOCUS:
        SetFocus( MainWindow );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

