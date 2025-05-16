#include <windows.h>
#include <ugctrl.h>

#include "..\include\visiparm.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\visigrid.h"
#include "..\include\wclass.h"

#include "ugtest.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS GridWindow;

static TCHAR DefaultFontName[] = TEXT( "MS Sans Serif" );

VISI_GRID_CLASS Grid;
int32        NofCols = 5;
int32        NofRows = 20;

TCHAR * RowName[] = {
    { "Fill Time" },
    { "Total Shot Time" },
    { "Fast Shot Time" },
    { "Slow Shot Time" },
    { "Cycle Time" },
    { "Avg Fast Shot Pressure" },
    { "Avg Slow Shot Pressure" },
    { "Metal Temperature" },
    { "Fast Shot Rise Time" },
    { "Biscuit Size" },
    { "Calc Start Fast Shot" },
    { "Intens Pressure" },
    { "Intens Rise Time" },
    { "Eff Peak Intens" },
    { "Impact Pressure To Peak" },
    { "Intens Squeeze Distance" },
    { "Sleeve Full Velocity" },
    { "Runner Full Velocity" },
    { "Avg Fill Velocity" },
    { "Fast Shot Distance" }
    };

TCHAR * ColName[] = {
    { "Average" },
    { "Std. Dev." },
    { "UCL" },
    { "LCL" },
    { "Cpk" },
    };

TCHAR MyName[]  = TEXT( "UGTest" );
TCHAR MyTitle[] = TEXT( "Ultimate Grid Test" );

/***********************************************************************
*                               FILL_GRID                              *
***********************************************************************/
static void fill_grid( void )
{
static BOOLEAN firstime = TRUE;

HDC     dc;
int32   col;
int32   row;
int32   maxwide;
int32   i;
int32   w;
RECT    r;
CUGCell cell;

if ( firstime )
    firstime = FALSE;

Grid.init( NofCols, NofRows );

Grid.QuickSetText( -1, -1, "What's Up" );

for ( col=0; col<NofCols; col++ )
    Grid.QuickSetText( col, -1, ColName[col] );

for ( row=0; row<NofRows; row++ )
    Grid.QuickSetText( -1, row, RowName[row] );

for ( row=0; row<NofRows; row++ )
    for ( col=0; col<NofCols; col++ )
        Grid.QuickSetText( col, row, "0.0" );

Grid.QuickSetText( 0, 0,"Don't Touch" );

cell.SetCellType( UGCT_DROPLIST );
cell.SetText( "Computer" );
cell.SetLabelText( "Computer\nZippy\n0.0" );
Grid.SetCell( 0, 1, &cell );

Grid.AdjustComponentSizes();
}

/***********************************************************************
*                         EDIT_DOWN_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK edit_down_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        GridWindow = hWnd;
        Grid.AttachGrid( hWnd, GRID_ID );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        MainWindow.shrinkwrap( hWnd );
        fill_grid();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                MainWindow.close();
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch (id)
            {
            case 108:
                MainWindow.close();
                return 0;
            }
        break;

    case WM_DBINIT:
        return 0;

    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( void )
{

WNDCLASS wc;

wc.lpszClassName = MyName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyName );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindowEx(
    WS_EX_APPWINDOW,
    MyName,
    MyTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("EDIT_DOWN_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) edit_down_dialog_proc
    );

MainWindow.show( SW_SHOW );
GridWindow.show( SW_SHOW );

UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup();

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && GridWindow.handle() )
        status = IsDialogMessage( GridWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
