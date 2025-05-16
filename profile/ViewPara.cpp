#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\subs.h"

#include "extern.h"

/*
------------------
Main menu controls
------------------ */
#define VIEW_PARAMETERS_MENU   507

/*
-----------
My controls
----------- */
#define PARAMETER_LISTBOX  101

#define MY_FLOAT_LEN 9
#define MAXLINE     100

const TCHAR NullChar = TEXT('\0');
const TCHAR TabChar  = TEXT('\11');

extern HWND MainWindow;
extern HWND MainPlotWindow;
extern HWND ViewParametersDialogWindow;

void toggle_menu_view_state( BOOLEAN & state, UINT menu_choice_id );
BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );

/***********************************************************************
*                   POSITION_VIEW_PARAMETERS_WINDOW                    *
***********************************************************************/
void position_view_parameters_window( void )
{
INT  myheight;
INT  mywidth;
RECT r;
RECT mr;

GetWindowRect( MainPlotWindow, &mr );
GetWindowRect( ViewParametersDialogWindow, &r );
mywidth  = r.right - r.left;
myheight = r.bottom - r.top;

MoveWindow( ViewParametersDialogWindow, mr.right-mywidth, mr.top, mywidth, myheight, TRUE );
}

/***********************************************************************
*                             DRAW_ONE_ITEM                            *
***********************************************************************/
static void draw_one_item( LPDRAWITEMSTRUCT dip )
{
HDC     dc;
TCHAR * cp;
PARAMETER_ENTRY * p;
TEXTMETRIC tm;
COLORREF old_text_color;
COLORREF my_text_color;
int pixels_per_char;
int i;
int j;
int x;
int y;
int dx;
float value;

static const int a[NOF_ALARM_LIMIT_TYPES] =
    { WARNING_MIN, WARNING_MAX, ALARM_MIN, ALARM_MAX };

dc = dip->hDC;

GetTextMetrics( dc, &tm );

pixels_per_char = tm.tmAveCharWidth;

y  = (dip->rcItem.bottom + dip->rcItem.top - tm.tmHeight) / 2;
x  = pixels_per_char / 4;

dx = PARAMETER_NAME_LEN * pixels_per_char;

i = dip->itemData;

if ( i < 0 || i >= MAX_PARMS )
    return;

p = &CurrentParam.parameter[i];

if ( p->input.type == NO_PARAMETER_TYPE )
    return;

TextOut( dc, x, y, p->name, lstrlen(p->name) );

x += dx;
dx = ( 11 ) * pixels_per_char;

for ( j=0; j<NOF_ALARM_LIMIT_TYPES; j++ )
    {
    cp = ascii_float( p->limits[a[j]].value );
    TextOut( dc, x, y, cp, lstrlen(cp) );
    x += dx;
    }

value = Parms[i];

if ( is_alarm(i, value) )
    my_text_color = AlarmColor;
else if ( is_warning(i, value) )
    my_text_color = WarningColor;
else
    my_text_color = DialogTextColor;

if ( my_text_color != 0 )
    old_text_color = SetTextColor( dc, my_text_color );

cp = ascii_float( value );
TextOut( dc, x, y, cp, lstrlen(cp) );
x += dx;

if ( my_text_color != 0 )
    SetTextColor( dc, old_text_color );

cp = units_name( p->units );
TextOut( dc, x, y, cp, lstrlen(cp) );
}

/***********************************************************************
*                     FILL_VIEW_PARAMETERS_LISTBOX                     *
***********************************************************************/
void fill_view_parameters_listbox( void )
{
int i;
int n;
LRESULT ti;

PARAMETER_ENTRY * p;

ti = SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_GETTOPINDEX, 0, 0L );

SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, WM_SETREDRAW, 0,  0l );
SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_RESETCONTENT, 0, 0L );

n = CurrentParam.count();

for ( i=0; i<n; i++ )
    {
    p = &CurrentParam.parameter[i];

    if ( p->input.type == NO_PARAMETER_TYPE )
        continue;

    SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_ADDSTRING, 0,(LPARAM) i );
    }

if ( ti != LB_ERR && ti < n )
    SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, LB_SETTOPINDEX, ti, 0L );

SendDlgItemMessage( ViewParametersDialogWindow, PARAMETER_LISTBOX, WM_SETREDRAW, 1,  0l );
InvalidateRect( GetDlgItem(ViewParametersDialogWindow, PARAMETER_LISTBOX), NULL, TRUE );

}

#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                      VIEW_PARAMETERS_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK view_parameters_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
LPMEASUREITEMSTRUCT mp;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_MEASUREITEM:
        mp = (LPMEASUREITEMSTRUCT) lParam;
        mp->itemHeight = 13;
        return TRUE;

    case WM_DRAWITEM:
        draw_one_item( (LPDRAWITEMSTRUCT) lParam );
        return TRUE;

    case WM_CLOSE:
        toggle_menu_view_state( ViewParametersDisplay, VIEW_PARAMETERS_MENU );
        ShowWindow( ViewParametersDialogWindow, SW_HIDE );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;
    }

return FALSE;
}
