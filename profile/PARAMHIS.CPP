#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\rectclas.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "extern.h"

/*
-----------
My Controls
----------- */
#define MIN_SHOT_TEXTBOX  201
#define MAX_SHOT_TEXTBOX  202

/*
------------------------
Active Region Definition
------------------------ */
#define LEFT_EDGE  5
int     Top_Edge = 10;

static long  DX       = 6;
static long  DY       = 13;

#define ALARM_COLOR_INDEX    0
#define NO_ALARM_COLOR_INDEX 1
#define WARNING_COLOR_INDEX  2

short     CurrentHistorySelect  = NO_INDEX;
int32     LowHistoryShotNumber  = NO_SHOT_NUMBER;
int32     HighHistoryShotNumber = NO_SHOT_NUMBER;

static float     * ParamArray      = 0;
static int32     * ShotNumberArray = 0;
static BITSETYPE * SaveFlagsArray  = 0;

extern int  LineHeight;

BOOLEAN is_alarm( short parameter_index, float value );
BOOLEAN is_warning( short parameter_index, float value );
RECT & my_desktop_client_rect();

/***********************************************************************
*                        CLEANUP_DYNAMIC_ARRAYS                        *
***********************************************************************/
static void cleanup_dynamic_arrays()
{
if ( ParamArray )
    {
    delete[] ParamArray;
    ParamArray = 0;
    }
if ( ShotNumberArray )
    {
    delete[] ShotNumberArray;
    ShotNumberArray = 0;
    }
if ( SaveFlagsArray )
    {
    delete[] SaveFlagsArray;
    SaveFlagsArray = 0;
    }
}

/***********************************************************************
*                        ALLOCATE_DYNAMIC_ARRAYS                       *
***********************************************************************/
static void allocate_dynamic_arrays()
{
int n;
if ( HistoryShotCount )
    {
    n = HistoryShotCount * MAX_PARMS;
    ParamArray = new float[n];
    ShotNumberArray = new int32[HistoryShotCount];
    SaveFlagsArray = new BITSETYPE[HistoryShotCount];
    }
}

/***********************************************************************
*                          NEW_HISTORY_CHOICE                          *
***********************************************************************/
static void new_history_choice()
{
BOOLEAN  have_new_shot;
NAME_CLASS s;
DB_TABLE t;
int32    n;
int32    i;
int32    shot_number;

have_new_shot = FALSE;

if ( !lstrlen(ComputerName) )
    return;

if ( !lstrlen(MachineName) )
    return;

if ( !lstrlen(PartName) )
    return;

s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( !s.file_exists() )
    return;

if ( !t.open( s.text(), GRAPHLST_RECLEN, PFL ) )
    return;

n = (int32) t.nof_recs();
if ( n > 0 )
    {
    i = n - HistoryShotCount;
    if ( i < 0 )
        i = 0;
    i += CurrentHistorySelect;

    if ( i >= n )
        i = n - 1;

    t.goto_record_number(i);
    t.get_rec();

    shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
    have_new_shot = TRUE;
    }
t.close();

if ( have_new_shot )
    {
    insalph( ShotName, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
    *(ShotName+SHOT_LEN) = TEXT('\0');
    SendMessage( MainWindow, WM_NEWSHOT, 0, 0L );
    }
}

/***********************************************************************
*                         PARAMETER_COLOR                              *
*                                                                      *
*         Return an index [0, 1, 2] = [Red, Green, Yellow]             *
***********************************************************************/
static short parameter_color( short parameter_index, float value )
{
if ( is_alarm( parameter_index, value) )
    return ALARM_COLOR_INDEX;

if ( is_warning( parameter_index, value) )
    return WARNING_COLOR_INDEX;

return NO_ALARM_COLOR_INDEX;
}

/***********************************************************************
*                    POSITION_PARAM_HISTORY_WINDOW                     *
***********************************************************************/
void position_param_history_window( void )
{

RECTANGLE_CLASS r;
RECTANGLE_CLASS rd;
WINDOW_CLASS    w;
int             x;

if ( !ParamHistoryWindow )
    return;

if ( !ParameterWindow )
    return;

GetWindowRect( ParameterWindow, &r );

r.left = r.right;

if ( IsFullScreen )
    rd = my_desktop_client_rect();
else
    rd = desktop_client_rect();

r.right = rd.right;

DY       = LineHeight; /* This is calculated in paramwin */
Top_Edge = DY;

if ( HistoryShotCount > 0 )
    {
    DX = r.right - r.left;
    DX -= 2*LEFT_EDGE;
    DX /= HistoryShotCount;
    }

w = GetDlgItem( ParamHistoryWindow, MAX_SHOT_TEXTBOX );
w.get_move_rect( rd );

x = LEFT_EDGE + HistoryShotCount*DX;
x -= rd.width();
w.move( x, 0 );

MoveWindow( ParamHistoryWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                         CHECK_CURRENT_SELECT                         *
***********************************************************************/
static BOOLEAN check_current_select( void )
{
int32 current_shot_number;
short slen;

slen = lstrlen( ShotName );
if ( !slen )
    return FALSE;

if ( LowHistoryShotNumber == NO_SHOT_NUMBER )
    return FALSE;

current_shot_number = extlong( ShotName, slen );

if ( current_shot_number < LowHistoryShotNumber )
    return FALSE;

if ( current_shot_number > HighHistoryShotNumber )
    return FALSE;

CurrentHistorySelect = current_shot_number - LowHistoryShotNumber;
return TRUE;
}

/***********************************************************************
*                 HIGHLIGHT_CURRENT_HISTORY_SELECTION                  *
***********************************************************************/
static void highlight_current_history_selection( HDC dc )
{
RECT r;
HPEN     oldpen;
HPEN     mypen;

r.top    = Top_Edge;
r.bottom = (NofCornerParameters + 1)*DY - 1;

if ( !check_current_select() )
    return;

r.left = LEFT_EDGE + CurrentHistorySelect*DX;
r.right = r.left + DX;
mypen = CreatePen( PS_SOLID, 2, RGB(255,255,255) );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );

    MoveToEx( dc, r.left,  r.top, 0 );
    LineTo(   dc, r.left,  r.bottom );
    LineTo(   dc, r.right, r.bottom );
    LineTo(   dc, r.right, r.top );
    LineTo(   dc, r.left, r.top );
    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

}

/***********************************************************************
*                    READ_PARAMETERS_FROM_PARMLIST                     *
***********************************************************************/
int32 read_parameters_from_parmlist( int32 shot_number, int nof_shots )
{
COMPUTER_CLASS c;
NAME_CLASS filename;
FIELDOFFSET  offset;
DB_TABLE     t;
int32        i;
int          n;
float      * pa;

n = 0;

if ( !c.online_status(ComputerName) )
    return MAX_PARMS;

if ( ParamArray )
    {
    pa = ParamArray;
    filename = shotparm_dbname( ComputerName, MachineName, PartName );
    if ( filename.file_exists() )
        {
        if ( t.open(filename.text(), SHOTPARM_RECLEN, PFL) )
            {
            t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
            t.reset_search_mode();
            if ( t.get_next_key_match(1, FALSE) )
                {
                while ( true )
                    {
                    offset = SHOTPARM_PARAM_1_OFFSET;
                    for ( i=0; i<MAX_PARMS; i++ )
                        {
                        pa[i] = t.get_float( offset );
                        offset += SHOTPARM_FLOAT_LEN+1;
                        }

                    n++;
                    if ( n == nof_shots )
                        break;

                    /*
                    ----------------------------------------------------
                    Move the array pointer to the next row of parameters
                    ---------------------------------------------------- */
                    pa += MAX_PARMS;

                    if ( !t.get_next_record(NO_LOCK) )
                        break;
                    }
                }
            t.close();
            }
        }
    }

return MAX_PARMS;
}

/***********************************************************************
*                        DRAW_PARAMETER_HISTORY                        *
***********************************************************************/
static void draw_parameter_history( HDC dc )
{
int b;
int row;
HBRUSH   oldbrush;
HBRUSH   brush[3];
RECT     r;
DB_TABLE t;
int32    n;
int32    i;
int32    ip;
HPEN        oldpen;
HPEN        mypen;
INT         x;
INT         y;
float     * parameter;
NAME_CLASS  s;
COMPUTER_CLASS c;

if ( !lstrlen(ComputerName) )
    return;

if ( !lstrlen(MachineName) )
    return;

if ( not_a_part(PartName) )
    return;

if ( !c.online_status(ComputerName) )
    return;

for ( i=0; i<HistoryShotCount; i++ )
    ShotNumberArray[i] = NO_SHOT_NUMBER;
s.get_graphlst_dbname( ComputerName, MachineName, PartName );
if ( !s.file_exists() )
    return;

if ( !t.open(s.text(), GRAPHLST_RECLEN, PFL ) )
    return;

mypen = CreatePen( PS_SOLID, 2, RGB(0,0,0) );
if ( mypen )
    {
    oldpen = (HPEN) SelectObject( dc, mypen );
    }

brush[0] = CreateSolidBrush( AlarmColor   );
brush[1] = CreateSolidBrush( NoAlarmColor );
brush[2] = CreateSolidBrush( WarningColor );
oldbrush = (HBRUSH) SelectObject( dc, brush[0] );

n = (int32) t.nof_recs();
i = n - HistoryShotCount;
if ( i < 0 )
    i = 0;
t.goto_record_number(i);
t.get_rec();
row = 0;
while ( i < n )
    {
    ShotNumberArray[row] = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
    SaveFlagsArray[row]  = (BITSETYPE) t.get_short( GRAPHLST_SAVE_FLAGS_OFFSET );
    i++;
    row++;
    if ( !t.get_next_record(NO_LOCK) )
        break;
    }
t.close();

/*
------------------------
Save the number of shots
------------------------ */
n = row;

if ( CurrentMachine.is_ftii )
    read_parameters_from_parmlist( ShotNumberArray[0], n );

parameter = ParamArray;

r.left = LEFT_EDGE;
for ( i=0; i<n; i++ )
    {
    if ( r.left == LEFT_EDGE )
        {
        LowHistoryShotNumber = ShotNumberArray[i];
        SetDlgItemInt( ParamHistoryWindow, MIN_SHOT_TEXTBOX, ShotNumberArray[i], TRUE );
        }
    r.top    = Top_Edge;
    r.right  = r.left + DX;

    if ( !CurrentMachine.is_ftii )
        read_parameters_from_profile( parameter, profile_name(ComputerName, MachineName, PartName, ShotNumberArray[i]) );

    for ( row=0; row<NofCornerParameters; row++ )
        {
        r.bottom = r.top + DY;

        b  = 1;
        ip = CornerParameters[row] - 1;
        if ( ip >= 0 && ip < MAX_PARMS )
            b = parameter_color( ip, parameter[ip] );

        FillRect( dc, &r, brush[b] );

        if ( mypen && row == 0 && (SaveFlagsArray[i] & SAVE_PROFILE_DATA) )
            {
            x = (r.left + r.right )/2 - 1;
            y = (r.top + r.bottom )/2 - 1;
            MoveToEx( dc, x, y,  0 );
            LineTo(   dc, x+1, y+1 );
            }

        r.top = r.bottom;
        }

    parameter += MAX_PARMS;
    r.left = r.right;
    }

i = n-1;
if ( ShotNumberArray[i] > NO_SHOT_NUMBER )
    {
    SetDlgItemInt( ParamHistoryWindow, MAX_SHOT_TEXTBOX, ShotNumberArray[i], TRUE );
    HighHistoryShotNumber = ShotNumberArray[i];
    }

SelectObject( dc, oldbrush );

for ( b=0; b<3; b++ )
    DeleteObject( brush[b] );

if ( mypen )
    {
    SelectObject( dc, oldpen );
    DeleteObject( mypen );
    }

highlight_current_history_selection( dc );
}

/***********************************************************************
*                           PAINT_MY_HISTORY                           *
***********************************************************************/
static void paint_my_history( void )
{

PAINTSTRUCT ps;

BeginPaint( ParamHistoryWindow, &ps );

draw_parameter_history( ps.hdc );

EndPaint( ParamHistoryWindow, &ps );
}

/***********************************************************************
*                         PARAMHISTORYPROC                             *
***********************************************************************/
int CALLBACK ParamHistoryProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int x;
HWND w;

switch (msg)
    {
    case WM_INITDIALOG:
        allocate_dynamic_arrays();
        return TRUE;

    case WM_LBUTTONDBLCLK:
        return TRUE;

    case WM_LBUTTONDOWN:
        x = LOWORD( lParam );
        x -= LEFT_EDGE;
        if ( x >= 0 )
            {
            x /= DX;
            if ( x < HistoryShotCount )
                {
                CurrentHistorySelect = x;
                new_history_choice();
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                }
            }
        if ( IsWindowVisible(MainWindow) )
            SetFocus( MainWindow );
        return TRUE;

    case WM_PAINT:
        paint_my_history();
        w = MainWindow;
        if ( ChosshotWindow )
            if ( IsWindowVisible(ChosshotWindow) )
                w = ChosshotWindow;
        if ( GetFocus() == hWnd )
            SetFocus( w );
        return TRUE;

    case WM_DESTROY:
        cleanup_dynamic_arrays();
        break;

    }

return FALSE;
}
