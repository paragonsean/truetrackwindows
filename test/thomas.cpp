#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "thomas.h"
#include "resrc1.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS BrouseWindow;

static TCHAR MyClassName[]     = "Thomas";
static TCHAR BackSlash[]       = "\\";
static TCHAR ExesDest[]        = "c:\\v5\\exes";
static TCHAR EditPartExe[]     = "\\EditPart.exe";
static TCHAR SetupSheetFile[]  = "\\SetupSheet.txt";
static TCHAR EditPartIniFile[] = TEXT( "\\editpart.ini" );
static TCHAR ConfigSection[]   = TEXT( "Config" );
static TCHAR ShowSetupSheetKey[] = TEXT( "ShowSetupSheet" );
static TCHAR SetupSheetLevelKey[]= TEXT( "SetupSheetLevel" );

static TCHAR MyExesDir[MAX_PATH+1];

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                           BROUSE_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK brouse_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int id;
int cmd;
HWND ws;
STRING_CLASS dest;
STRING_CLASS sorc;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        BrouseWindow = hWnd;
        MainWindow.shrinkwrap( hWnd );
        dest = ExesDest;
        dest += EditPartExe;
        if ( !file_exists(dest.text()) )
            {
            *ExesDest = TEXT( 'd' );
            dest = ExesDest;
            dest += EditPartExe;
            if ( !file_exists(dest.text()) )
                {
                MessageBox( hWnd, "Unable to find exes folder", "Unable to continue", MB_OK );
                }
            }

        set_text( hWnd, EXES_PATH_TBOX, ExesDest );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( get_exe_directory(MyExesDir) )
                    {
                    sorc = MyExesDir;
                    sorc += EditPartExe;
                    dest = ExesDest;
                    dest += EditPartExe;
                    CopyFile( sorc.text(), dest.text(), FALSE );

                    sorc = MyExesDir;
                    sorc += SetupSheetFile;
                    dest = ExesDest;
                    dest += SetupSheetFile;
                    CopyFile( sorc.text(), dest.text(), FALSE );

                    dest = ExesDest;
                    dest += EditPartIniFile;
                    WritePrivateProfileString( ConfigSection, ShowSetupSheetKey,  TEXT("Y"),  dest.text() );
                    WritePrivateProfileString( ConfigSection, SetupSheetLevelKey, TEXT("10"), dest.text() );
                    }

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

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
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
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT( "BROUSE_DIALOG" ),
    MainWindow.handle(),
    (DLGPROC) brouse_dialog_proc );

MainWindow.show( cmd_show );
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
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;
BOOL status;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( BrouseWindow.handle() )
        status = IsDialogMessage( BrouseWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return( msg.wParam );
}
