#include <windows.h>
#include <shlobj.h>

#include "..\include\visiparm.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS BrouseWindow;
LISTBOX_CLASS Lb;

static TCHAR MyClassName[] = TEXT( "NetSetup" );

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                            WAIT_FOR_FILE                             *
*        Wait for a file to appear (e.g. after CopyFile).              *
***********************************************************************/
static boolean wait_for_file( TCHAR * filename )
{
int count;

count = 10;
while ( true )
    {
    if ( file_exists(filename) )
        break;

    count--;
    if ( count == 0 )
        return false;

    Sleep( 500 );
    }

return true;
}

/***********************************************************************
*                     SCROLL_LISTBOX_TO_LAST_ENTRY                     *
***********************************************************************/
static void scroll_listbox_to_last_entry()
{
int ti;

ti = Lb.count() - Lb.visible_row_count();

if ( ti > Lb.top_index() )
    Lb.set_top_index( ti );
}

/***********************************************************************
*                         CREATE_DESKTOP_LINK                          *
*  The title is what will appear on the desktop,                       *
*  e.g., Data Archive Restore                                          *
*                                                                      *
*  the link_name is the name of the link itself                        *
*  e.g. "data archive restore.lnk"                                     *
*                                                                      *
*  the exe_path is the full path to the executable                     *
*  e.g. C:\V5\EXES\DSRESTORE.EXE                                       *
***********************************************************************/
static void create_desktop_link( TCHAR * title, TCHAR * link_name, TCHAR * exe_path )
{
static TCHAR ShellFolderKey[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders" );

TCHAR buf[MAX_PATH];
WCHAR ws[MAX_PATH];

ULONG buf_size = MAX_PATH;
HKEY  hCU;
DWORD lpType;

HRESULT        result;
IShellLink   * psl;
IPersistFile * ppf;
bool           have_key;

/*
---------------------------
Get the path of the desktop
--------------------------- */
have_key = false;
if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
    {
    buf_size = MAX_PATH * sizeof(TCHAR);
    if ( RegQueryValueEx( hCU, TEXT( "Common Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
        have_key = true;
    RegCloseKey(hCU);
    }

/*
-----------------------------------------
Win95 doesn't normally have the above key
----------------------------------------- */
if ( !have_key )
    {
    if ( RegOpenKeyEx(HKEY_CURRENT_USER, ShellFolderKey, 0, KEY_QUERY_VALUE, &hCU) == ERROR_SUCCESS )
        {
        buf_size = MAX_PATH * sizeof(TCHAR);
        if ( RegQueryValueEx( hCU, TEXT("Desktop"), NULL, &lpType, (unsigned char *) &buf, &buf_size) == ERROR_SUCCESS )
            have_key = true;
        RegCloseKey(hCU);
        }
    }

append_filename_to_path(  buf, link_name );

if ( CoInitialize(0) == S_OK )
    {
    /*
    ------------------------------------------
    Get a pointer to the IShellLink interface.
    ------------------------------------------ */
    result = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &psl );
    if (SUCCEEDED(result) )
        {
        /*
        ------------------------------------------------------------------------------------------------
        Query IShellLink for the IPersistFile interface for saving the shell link in persistent storage.
        ------------------------------------------------------------------------------------------------ */
        result = psl->QueryInterface( IID_IPersistFile, (void**) &ppf );
        if ( SUCCEEDED(result) )
            {
            /*
            --------------------------------------
            Set the path to the shell link target.
            -------------------------------------- */
            psl->SetPath( exe_path );

            /*
            --------------------------------------
            Set the description of the shell link.
            -------------------------------------- */
            psl->SetDescription( title );

            /*
            ----------------------
            Ensure string is ANSI.
            ---------------------- */
            #ifdef UNICODE
                lstrcpy( ws, buf );
            #else
                MultiByteToWideChar(CP_ACP, 0, buf, -1, ws, MAX_PATH);
            #endif

            /*
            ------------------------------------------------
            Save the link via the IPersistFile::Save method.
            ------------------------------------------------ */
            result = ppf->Save(ws, TRUE);

            /*
            --------------------------------
            Release pointer to IPersistFile.
            -------------------------------- */
            ppf->Release();
            }

        /*
        ------------------------------
        Release pointer to IShellLink.
        ------------------------------ */
        psl->Release();
        }

    CoUninitialize();
    }
}

/***********************************************************************
*                                BEGIN                                 *
***********************************************************************/
static void begin()
{
static TCHAR NetSetupTitle[]     = TEXT( "NetSetup" );
static TCHAR NetSetupLinkName[]  = TEXT( "NetSetup.lnk" );
static TCHAR NetSetupExePath[]   = TEXT( "c:\\visinet\\netsetup.exe" );
static TCHAR NetSetupExeFile[]   = TEXT( "netsetup.exe" );
static TCHAR NetSetupDirectory[] = TEXT( "c:\\visinet" );

static TCHAR NetSetupHelpPath[]   = TEXT( "c:\\visinet\\netsetup.chm" );
static TCHAR NetSetupHelpFile[]   = TEXT( "netsetup.chm" );

TCHAR sorc[MAX_PATH];

if ( !directory_exists(NetSetupDirectory) )
    {
    Lb.add( TEXT("Creating the VisiNet Folder...") );
    create_directory( NetSetupDirectory );
    }

/*
------------------
Copy the help file
------------------ */
Lb.add( TEXT("Copying NetSetup help file...") );
lstrcpy( sorc, exe_directory() );
append_filename_to_path( sorc, NetSetupHelpFile );
if ( !CopyFile( sorc, NetSetupHelpPath, FALSE) )
    Lb.add( TEXT("Unable to copy NetSetup.chm!") );

/*
---------------------
Copy the program file
--------------------- */
Lb.add( TEXT("Copying NetSetup program...") );
lstrcpy( sorc, exe_directory() );
append_filename_to_path( sorc, NetSetupExeFile );
if ( !CopyFile( sorc, NetSetupExePath, FALSE) )
    {
    Lb.add( TEXT("Unable to copy NetSetup.exe!") );
    }
else
    {
    Lb.add( TEXT("Waiting for copy to finish...") );
    wait_for_file( NetSetupExePath );

    /*
    -----------------------------------
    Remove the read-only file attribute
    ----------------------------------- */
    SetFileAttributes( NetSetupExePath, FILE_ATTRIBUTE_NORMAL );

    Lb.add( TEXT("Creating a desktop link to the NetSetup Program") );
    create_desktop_link( NetSetupTitle, NetSetupLinkName, NetSetupExePath );
    }

Lb.add( TEXT("Finished, press [ESC] to exit") );
scroll_listbox_to_last_entry();
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
HWND begin_button;
HWND w;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;

        begin_button = CreateWindow(
            "BUTTON",
            "Begin",
            WS_CHILD | BS_NOTIFY | BS_PUSHBUTTON | WS_VISIBLE,
            200, 145,
            100, 25,
            hWnd,
            (HMENU) 100,
            MainInstance,
            0
            );

        w = CreateWindow(
            "LISTBOX",
            0,
            WS_CHILD | LBS_NOSEL | WS_VISIBLE,
            3, 0,
            490, 143,
            hWnd,
            (HMENU) STATUS_LISTBOX,
            MainInstance,
            0
            );

        Lb.init( w );
        Lb.add( TEXT("Press 'Begin' to install the NetSetup software") );
        SetFocus( begin_button );
        break;

    case WM_CHAR:
        if ( wParam == VK_ESCAPE )
            {
            MainWindow.close();
            return 0;
            }
        break;

    case WM_COMMAND:
        cmd  = HIWORD( wParam );
        switch (id)
            {
            case BEGIN_BUTTON:
                if ( cmd == BN_CLICKED )
                    {
                    EnableWindow( GetDlgItem(hWnd, BEGIN_BUTTON), FALSE );
                    MainWindow.set_focus();
                    begin();
                    }
                return 0;
            }

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
    505, 200,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

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

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

shutdown();
return( msg.wParam );
}
