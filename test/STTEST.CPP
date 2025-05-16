#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#include "sttest.h"

BOOL CALLBACK velocity_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK pressure_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK gp_variable_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

struct WINDOW_ENTRY
    {
    TCHAR * name;
    DLGPROC proc;
    HWND    handle;
    };

WINDOW_ENTRY WindowInfo[] = {
    { TEXT("VELOCITY_CONTROL_DIALOG"),         velocity_control_dialog_proc, 0 },
    { TEXT("PRESSURE_CONTROL_DIALOG"),         pressure_control_dialog_proc, 0 },
    { TEXT("GENERAL_PURPOSE_VARIABLE_DIALOG"), gp_variable_dialog_proc,      0 }
    };

int TAB_COUNT = sizeof(WindowInfo)/sizeof(WINDOW_ENTRY);

static int NO_TAB    = -1;
static int VELOCITY_CONTROL_TAB    = 0;
static int PRESSURE_CONTROL_TAB    = 1;
static int GP_VARIABLE_TAB         = 2;
static int CurrentDialogIndex = NO_TAB;

HINSTANCE MainInstance;
HWND      MainWindow;

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                   VELOCITY_CONTROL_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK velocity_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        break;

    case WM_DESTROY:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                   PRESSURE_CONTROL_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK pressure_control_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        break;

    case WM_DESTROY:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                       GP_VARIABLE_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK gp_variable_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        break;

    case WM_DESTROY:
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                DestroyWindow( hWnd );
                break;
            }
    break;
    }


return FALSE;
}

/***********************************************************************
*                          SHOW_CURRENT_DIALOG                         *
***********************************************************************/
static void show_current_dialog( int new_dialog_index )
{
int id;

if ( new_dialog_index == CurrentDialogIndex )
    {
    return;
    }

if ( CurrentDialogIndex != NO_TAB )
    {
    ShowWindow( WindowInfo[CurrentDialogIndex].handle, SW_HIDE );
    }

if ( new_dialog_index >= 0 && new_dialog_index < TAB_COUNT )
    {
    CurrentDialogIndex = new_dialog_index;
    ShowWindow( WindowInfo[CurrentDialogIndex].handle, SW_SHOW );
    if ( new_dialog_index == PRESSURE_CONTROL_TAB )
        id = VELOCITY_CONTROL_TAB_STRING;
    else if ( new_dialog_index == VELOCITY_CONTROL_TAB )
        id = PRESSURE_CONTROL_TAB_STRING;
    else
        id = NO_INDEX;
 
    if ( id != NO_INDEX )
        set_text( MainWindow, VELOCITY_CONTROL_BUTTON, resource_string(id) );
    }
}

/***********************************************************************
*                            CREATE_DIALOGS                            *
***********************************************************************/
static void create_dialogs( void )
{
int i;

for ( i=0; i<TAB_COUNT; i++ )
    {
    WindowInfo[i].handle = CreateDialog(
        MainInstance,
        WindowInfo[i].name,
        MainWindow,
        WindowInfo[i].proc );
    }

}

/***********************************************************************
*                         POSITION_MAIN_WINDOW                         *
***********************************************************************/
void position_main_window( void )
{
RECT cr;
RECT r;
long width;
HWND w;
long i;

GetWindowRect( MainWindow, &r );
cr = desktop_client_rect();
OffsetRect( &r, cr.left - r.left, cr.bottom - r.bottom );
width = cr.right - cr.left;
MoveWindow( MainWindow, r.left, r.top, width, r.bottom-r.top, TRUE );

/*
--------------------------------------------------------
Reduce the width by the right edge of one of the buttons
-------------------------------------------------------- */
w = GetDlgItem( MainWindow, VELOCITY_CONTROL_BUTTON );
GetWindowRect( w, &r );
width -= r.right;

/*
-------------------------------------------------------------
All the edit windows are the same size. Center from the right
edge of the button.
------------------------------------------------------------- */
GetWindowRect( WindowInfo[VELOCITY_CONTROL_TAB].handle, &cr );

/*
------------------------------------------------
Reduce the width by the width of the edit window
------------------------------------------------ */
width += cr.left;
width -= cr.right;

/*
-----------------
Center the window
----------------- */
width /= 2;
i = cr.right - cr.left;
cr.left = r.right + width;
width = i;
if ( cr.left < r.right )
    cr.left = r.right;

for ( i=0; i<TAB_COUNT; i++ )
    MoveWindow( WindowInfo[i].handle, cr.left, 0, width, cr.bottom-cr.top, TRUE );
}

/***********************************************************************
*                          STTEST_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK sttest_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  dn;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MainWindow = hWnd;
        create_dialogs();
        position_main_window();
        show_current_dialog( VELOCITY_CONTROL_TAB );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case VELOCITY_CONTROL_BUTTON:
                if ( CurrentDialogIndex ==  VELOCITY_CONTROL_TAB )
                    dn = PRESSURE_CONTROL_TAB;
                else
                    dn = VELOCITY_CONTROL_TAB;
                show_current_dialog( dn );
                return TRUE;

            case GP_VARIABLE_BUTTON:
                show_current_dialog( GP_VARIABLE_TAB );
                return TRUE;

            case IDCANCEL:
                MainWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        MainWindow = 0;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
void startup( void )
{
InitCommonControls();
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MainInstance = this_instance;

startup();

DialogBox( MainInstance, TEXT("STTEST_DIALOG"), NULL,(DLGPROC) sttest_dialog_proc );

shutdown();
return 0;
}
