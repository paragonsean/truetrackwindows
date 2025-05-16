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

TCHAR SpaceChar = TEXT( ' ' );
static BOOLEAN HaveV5ds = FALSE;
static TCHAR MyClassName[]     = TEXT( "Test" );
static TCHAR VisiTrakIniFile[] = TEXT( "c:\\v5\\exes\\visitrak.ini" );

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
*                            COPY_ONE_FILE                             *
***********************************************************************/
BOOL copy_one_file( TCHAR * dest_dir, TCHAR * file_to_copy )
{
static TCHAR DotDotDot[] = TEXT( "..." );
static TCHAR Waiting[]   = TEXT( "Waiting for copy to finish..." );

TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * cp;
BOOL copy_ok;

/*
--------------------------------
Tell the human what is happening
-------------------------------- */
cp = copystring( sorc, TEXT("Copying ") );
cp = copystring( cp,   file_to_copy );
copystring( cp, DotDotDot );
Lb.add( sorc );

/*
----------------------------------------------------
The source file is in the same dir as the executable
---------------------------------------------------- */
copystring( sorc, exe_directory() );
append_filename_to_path( sorc, file_to_copy );

copystring( dest, dest_dir );
append_filename_to_path( dest, file_to_copy );

copy_ok = CopyFile( sorc, dest, FALSE );

if ( copy_ok )
    {
    Lb.add( TEXT("Waiting for copy to finish...") );
    wait_for_file( dest );

    /*
    -----------------------------------
    Remove the read-only file attribute
    ----------------------------------- */
    SetFileAttributes( dest, FILE_ATTRIBUTE_NORMAL );
    }
else
    {
    Lb.add( TEXT("Copy FAILED!") );
    }

return copy_ok;
}

/***********************************************************************
*                                BEGIN                                 *
***********************************************************************/
static void begin()
{
static TCHAR V5dsExesPath[]   = TEXT( "c:\\v5ds\\exes\\" );
static TCHAR V5ExesPath[]     = TEXT( "c:\\v5\\exes\\" );
static TCHAR RestoreDirPath[] = TEXT( "c:\\v5backup" );

static TCHAR BackupTitle[]     = TEXT( "Data Archiver Backup" );
static TCHAR BackupLinkName[]  = TEXT( "Data Archiver Backup.lnk" );
static TCHAR BackupChmFile[]   = TEXT( "dsbackup.chm" );
static TCHAR BackupExeFile[]   = TEXT( "dsbackup.exe" );
static TCHAR BackupIniFile[]   = TEXT( "dsbackup.ini" );

static TCHAR RestoreTitle[]    = TEXT( "Data Archiver Restore" );
static TCHAR RestoreLinkName[] = TEXT( "Data Archiver Restore.lnk" );
static TCHAR RestoreExePath[]  = TEXT( "c:\\v5\\exes\\dsrestor.exe" );
static TCHAR RestoreExeFile[]  = TEXT( "dsrestor.exe" );

static TCHAR VtExtracChmFile[]  = TEXT( "vtextrac.chm" );
static TCHAR VtExtracExeFile[]  = TEXT( "vtextrac.exe" );
static TCHAR VtExtracIniFile[]  = TEXT( "vtextrac.ini" );

INI_CLASS ini;
TCHAR sorc[MAX_PATH+1];

/*
----------------------
Copy the dsbackup file
---------------------- */
if ( copy_one_file(V5dsExesPath, BackupExeFile) )
    {
    copystring( sorc, V5dsExesPath );
    append_filename_to_path( sorc, BackupExeFile );
    Lb.add( TEXT("Creating a desktop link to the Data Archiver Backup Program") );
    create_desktop_link( BackupTitle, BackupLinkName, sorc );
    HaveV5ds = TRUE;


    copy_one_file( V5dsExesPath, BackupChmFile );
    /*
    --------------------------------------------------------
    Only copy the backup ini file if there isn't one already
    -------------------------------------------------------- */
    copystring( sorc, V5dsExesPath );
    append_filename_to_path( sorc, BackupIniFile );
    if ( !file_exists( sorc ) )
        copy_one_file( V5dsExesPath, BackupIniFile );

    /*
    ----------------------
    Copy the vtextrac file
    ---------------------- */
    copy_one_file( V5dsExesPath, VtExtracChmFile );
    copy_one_file( V5dsExesPath, VtExtracExeFile );

    /*
    --------------------------------------------------
    Only copy the VtExtrac Ini file if there isn't one
    -------------------------------------------------- */
    copystring( sorc, V5dsExesPath );
    append_filename_to_path( sorc, VtExtracIniFile );
    if ( !file_exists( sorc ) )
        copy_one_file( V5dsExesPath, VtExtracIniFile );
    }

/*
----------------------
Copy the dsrestor file
---------------------- */
if ( copy_one_file(V5ExesPath, RestoreExeFile) )
    {
    copystring( sorc, V5ExesPath );
    append_filename_to_path( sorc, RestoreExeFile );
    Lb.add( TEXT("Creating a desktop link to the Data Archiver Restore Program") );
    create_desktop_link( RestoreTitle, RestoreLinkName, sorc );

    if ( HaveV5ds )
        {
        Lb.add( TEXT("Creating the default restore folder [C:\\V5Backup]") );
        create_directory( RestoreDirPath );

        Lb.add( TEXT("Saving restore folder name in the Visitrak.ini file...") );
        ini.set_file( VisiTrakIniFile );
        ini.set_section( TEXT("Restore") );
        if ( !ini.find( TEXT("StartDir") ) )
            ini.put_string( TEXT("StartDir"),   RestoreDirPath );
        if ( !ini.find( TEXT("RestoreDir") ) )
            ini.put_string( TEXT("RestoreDir"), RestoreDirPath );
        }
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
            200, 446,
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
            490, 445,
            hWnd,
            (HMENU) STATUS_LISTBOX,
            MainInstance,
            0
            );

        Lb.init( w );
        Lb.add( TEXT("Press 'Begin' to install the software") );
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
    505, 500,                         // W,h
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
