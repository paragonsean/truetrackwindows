#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "resource.h"

#define _MAIN_
#include "extern.h"

#define BACKUP_HELP   0x0001
#define LOAD_ZIP_HELP 0x0002
#define RESTORE_HELP  0x0003
#define DELETE_HELP   0x0004
#define CONTENTS_HELP 0x0005

#define BACKUP_POPUP_HELP    0x0006
#define RESTORE_POPUP_HELP   0x0007
#define DELETE_POPUP_HELP    0x0008
#define LOAD_ZIP_POPUP_HELP  0x0009
#define BACKUP_LIST_HELP     10
#define BACKUP_LIST_POPUP_HELP     11

#define WM_HELP     0x0053

#define BACKUP_DIALOG_INDEX        0
#define BACKUP_LIST_DIALOG_INDEX   1
#define RESTORE_DIALOG_INDEX       2
#define DELETE_DIALOG_INDEX        3
#define DIALOG_COUNT               4

#define CHOOSE_BUTTON_COUNT   5

static TCHAR YChar    = TEXT( 'Y' );
static TCHAR NChar    = TEXT( 'N' );
static TCHAR NullChar = TEXT( '\0' );
static TCHAR ZeroChar = TEXT( '0' );
static TCHAR NineChar = TEXT( '9' );

HWND CurrentWindow = 0;
HWND MyWin[DIALOG_COUNT] = { 0, 0, 0, 0 };

HWND ChooseFunctionWindow = 0;
HWND ChooseButton[CHOOSE_BUTTON_COUNT] = {0, 0, 0, 0, 0};

TCHAR MyClassName[] = TEXT( "TrueTrakBackUp" );

TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
TCHAR BupresHelpFile[]  = TEXT( "bupres.hlp" );

TCHAR BackupSection[]   = TEXT( "Backup" );
TCHAR ConfigSection[]   = TEXT( "Config" );

BOOL CALLBACK backup_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL CALLBACK backup_list_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL CALLBACK delete_data_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL CALLBACK restore_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL CALLBACK SavingProgressProc(  HWND w, UINT msg, WPARAM wparam, LPARAM lparam );

/***********************************************************************
*                           SHOW_ONE_WINDOW                            *
***********************************************************************/
void show_one_window( HWND w )
{
int  i;
UINT si;

if ( CurrentWindow == w )
    return;

if ( CurrentWindow )
    {
    ShowWindow( CurrentWindow, SW_HIDE );
    SendMessage( CurrentWindow, WM_DBCLOSE, 0, 0L );
    }

SendMessage( w, WM_DBINIT, 0, 0L );
shrinkwrap( MainWindow, w );
ShowWindow( w, SW_SHOW );
CurrentWindow = w;
si = MAIN_CAPTION_STRING;

for ( i=0; i<DIALOG_COUNT; i++ )
    {
    if ( w == MyWin[i] )
        {
        si = BACKUP_DATA_STRING + i;
        break;
        }
    }

SetWindowText( MainWindow, resource_string(MainInstance, si) );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me( void )
{

PAINTSTRUCT ps;

BeginPaint( MainWindow, &ps );
EndPaint( MainWindow, &ps );

}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
BOOL show_context_help( HWND w )
{
int i;
DWORD context;
TCHAR fname[MAX_PATH+1];
static DWORD mywin_context[CHOOSE_BUTTON_COUNT] =
    {
    BACKUP_POPUP_HELP,
    BACKUP_LIST_POPUP_HELP,
    RESTORE_POPUP_HELP,
    DELETE_POPUP_HELP,
    LOAD_ZIP_POPUP_HELP
    };

context = CONTENTS_HELP;

for ( i=0; i<CHOOSE_BUTTON_COUNT; i++ )
    {
    if ( w == ChooseButton[i] )
        {
        context = mywin_context[i];
        break;
        }
    }

if ( context != CONTENTS_HELP )
    if ( get_exe_directory(fname) )
        {
        lstrcat( fname, BupresHelpFile );
        WinHelp( MainWindow, fname, HELP_CONTEXTPOPUP, context );
        return TRUE;
        }

return FALSE;
}

/***********************************************************************
*                           MAKE_BUTTON_LIST                           *
***********************************************************************/
void make_button_list( HWND w )
{
UINT i;
UINT x;

/*
-------------------------------------------------------------
Get the window handle of each of the choose function buttons;
------------------------------------------------------------- */
for ( i=0; i<CHOOSE_BUTTON_COUNT; i++ )
    {
    x = BACKUP_BUTTON + i;
    ChooseButton[i] = GetDlgItem( w, x );
    }

}

/***********************************************************************
*                         CHOOSE_FUNCTION_PROC                         *
***********************************************************************/
BOOL CALLBACK choose_function_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int i;
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        shrinkwrap( MainWindow, hWnd );
        make_button_list(hWnd);
        SetFocus( GetDlgItem(hWnd, BACKUP_BUTTON) );
        return TRUE;

    case WM_DBINIT:
        return TRUE;

    case WM_DBCLOSE:
        return TRUE;

    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam );

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case BACKUP_BUTTON:
            case BACKUP_LIST_BUTTON:
            case RESTORE_BUTTON:
            case DELETE_BUTTON:
                i = id - BACKUP_BUTTON;
                show_one_window( MyWin[i] );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
void gethelp( void )
{
int   i;
DWORD context;
TCHAR fname[MAX_PATH+1];
static DWORD mywin_context[DIALOG_COUNT] = { BACKUP_HELP, BACKUP_LIST_HELP, RESTORE_HELP, DELETE_HELP };

context = CONTENTS_HELP;

for ( i=0; i<DIALOG_COUNT; i++ )
    {
    if ( CurrentWindow == MyWin[i] )
        {
        context = mywin_context[i];
        break;
        }
    }

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, BupresHelpFile );
    WinHelp( MainWindow, fname, HELP_CONTEXT, context );
    }

}

/***********************************************************************
*                      NEW_MACHINE_NOTIFY_DIALOG_PROC                  *
***********************************************************************/
BOOL CALLBACK new_machine_notify_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}


/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

switch (msg)
    {
    case WM_CLOSE:
        if ( CurrentWindow == ChooseFunctionWindow )
            {
            if ( CreatedNewMachine )
                {
                DialogBox(
                    MainInstance,
                    TEXT("NEW_MACHINE_NOTIFY_DIALOG"),
                    hWnd,
                    (DLGPROC) new_machine_notify_dialog_proc
                    );
                }
            SendMessage( hWnd, WM_DESTROY, 0, 0L );
            }
        else
            show_one_window( ChooseFunctionWindow );
        return 0;

    case WM_CONTEXTMENU:
    case WM_HELP:
         gethelp();
         return 0;

    case WM_SETFOCUS:
        if ( CurrentWindow )
            SetFocus( CurrentWindow );
        return 0;

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}


/***********************************************************************
*                        GET_BACKUP_DIRECTORY                          *
***********************************************************************/
BOOLEAN get_backup_directory( void )
{

TCHAR * cp;
cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BackupDir") );
if ( lstrcmp(cp, UNKNOWN) != 0 )
    {
    lstrcpy( BackupDir, cp );
    return TRUE;
    }

*BackupDir = NullChar;
return FALSE;
}

/***********************************************************************
*                        RESTORE_CHECKED_STATE                         *
***********************************************************************/
static void restore_checked_state( HWND w, INT box, TCHAR * section, TCHAR * key )
{
TCHAR * cp;

cp = get_ini_string( VisiTrakIniFile, section, key );
if ( *cp == YChar )
    CheckDlgButton( w, box, BST_CHECKED );

}

/***********************************************************************
*                            RESTORE_CONFIG                            *
***********************************************************************/
void restore_config( void )
{
TCHAR * cp;
HWND w;

w = MyWin[BACKUP_DIALOG_INDEX];
restore_checked_state( w, TB_PURGE_CHECKBOX,          BackupSection, TEXT("BDPurge") );
restore_checked_state( w, TB_LAST_50_CHECKBOX,        BackupSection, TEXT("BDLast50") );
restore_checked_state( w, TB_PURGE_DOWNTIME_CHECKBOX, BackupSection, TEXT("BDPurgeDown") );

restore_checked_state( w, TB_SETUPS_CHECKBOX,     BackupSection, TEXT("BDSetups")    );
restore_checked_state( w, TB_SCRAP_DOWN_CHECKBOX, BackupSection, TEXT("BDScrapDown") );
restore_checked_state( w, TB_PROFILE_CHECKBOX,    BackupSection, TEXT("BDProfile")   );
restore_checked_state( w, TB_PARAM_CHECKBOX,      BackupSection, TEXT("BDParam")     );
restore_checked_state( w, TB_ALARM_CHECKBOX,      BackupSection, TEXT("BDAlarm")     );

cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BDZipName") );
if ( cp )
    SetDlgItemText( w, TB_ZIP_NAME_EDITBOX, cp );

w = MyWin[BACKUP_LIST_DIALOG_INDEX];
cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BListDZipName") );
if ( cp )
    {
    if ( !unknown(cp) )
        SetDlgItemText( w, TB_ZIP_NAME_EDITBOX, cp );
    }
w = MyWin[RESTORE_DIALOG_INDEX];
restore_checked_state( w, TR_SETUPS_CHECKBOX,     BackupSection, TEXT("RDSetups")    );
restore_checked_state( w, TR_SCRAP_DOWN_CHECKBOX, BackupSection, TEXT("RDScrapDown") );
restore_checked_state( w, TR_PROFILE_CHECKBOX,    BackupSection, TEXT("RDProfile")   );
restore_checked_state( w, TR_PARAM_CHECKBOX,      BackupSection, TEXT("RDParam")     );
restore_checked_state( w, TR_ALARM_CHECKBOX,      BackupSection, TEXT("RDAlarm")     );
if ( HaveSureTrakControl )
    restore_checked_state( w, TR_SURETRAK_CHECKBOX,   BackupSection, TEXT("RDSureTrak")  );

w = MyWin[DELETE_DIALOG_INDEX];
restore_checked_state( w, DD_PARAMETERS_CHECKBOX,  BackupSection, TEXT("DDParams")   );
restore_checked_state( w, DD_PROFILES_CHECKBOX,    BackupSection, TEXT("DDProfiles") );
restore_checked_state( w, DD_DELETE_RADIOBUTTON,   BackupSection, TEXT("DDDelete")   );
restore_checked_state( w, DD_RENUMBER_RADIOBUTTON, BackupSection, TEXT("DDRenumber") );

cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("BDPurge") );
if ( *cp == TEXT('Y') )
    {
    CheckDlgButton( MyWin[BACKUP_DIALOG_INDEX], TB_PURGE_CHECKBOX, BST_CHECKED );
    }

w = GetDlgItem( MyWin[DELETE_DIALOG_INDEX], DD_RENUMBER_EDITBOX );
cp = get_ini_string( VisiTrakIniFile, BackupSection, TEXT("DDFirstNum") );
if ( cp && (*cp >= ZeroChar && *cp <= NineChar) )
    SendMessage( w, WM_SETTEXT, 0, (LPARAM) cp );
else
    SendMessage( w, WM_SETTEXT, 0, (LPARAM) TEXT("1") );

}

/***********************************************************************
*                         SAVE_CHECKED_STATE                           *
***********************************************************************/
static void save_checked_state( TCHAR * section, TCHAR * key, HWND w, INT box )
{
static TCHAR checkedchar[] = TEXT( "Y" );

if ( is_checked(w, box) )
    *checkedchar = YChar;
else
    *checkedchar = NChar;

put_ini_string( VisiTrakIniFile, section, key, checkedchar );
}

/***********************************************************************
*                             SAVE_CONFIG                              *
***********************************************************************/
void save_config( void )
{
HWND w;
TCHAR buf[MAX_INTEGER_LEN+1];
TCHAR pbuf[MAX_INI_STRING_LEN+1];

w = MyWin[BACKUP_DIALOG_INDEX];
save_checked_state( BackupSection, TEXT("BDPurge"),     w, TB_PURGE_CHECKBOX );
save_checked_state( BackupSection, TEXT("BDLast50"),    w, TB_LAST_50_CHECKBOX );
save_checked_state( BackupSection, TEXT("BDPurgeDown"), w, TB_PURGE_DOWNTIME_CHECKBOX );

save_checked_state( BackupSection, TEXT("BDSetups"),    w, TB_SETUPS_CHECKBOX     );
save_checked_state( BackupSection, TEXT("BDScrapDown"), w, TB_SCRAP_DOWN_CHECKBOX );
save_checked_state( BackupSection, TEXT("BDProfile"),   w, TB_PROFILE_CHECKBOX    );
save_checked_state( BackupSection, TEXT("BDParam"),     w, TB_PARAM_CHECKBOX      );
save_checked_state( BackupSection, TEXT("BDAlarm"),     w, TB_ALARM_CHECKBOX      );

if ( get_text( pbuf, w, TB_ZIP_NAME_EDITBOX, MAX_INI_STRING_LEN) )
    put_ini_string( VisiTrakIniFile, BackupSection, TEXT("BDZipName"), pbuf );

w = MyWin[BACKUP_LIST_DIALOG_INDEX];
if ( get_text( pbuf, w, TB_ZIP_NAME_EDITBOX, MAX_INI_STRING_LEN) )
    put_ini_string( VisiTrakIniFile, BackupSection, TEXT("BListDZipName"), pbuf );

w = MyWin[RESTORE_DIALOG_INDEX];
save_checked_state( BackupSection, TEXT("RDSetups"),    w, TR_SETUPS_CHECKBOX     );
save_checked_state( BackupSection, TEXT("RDScrapDown"), w, TR_SCRAP_DOWN_CHECKBOX );
save_checked_state( BackupSection, TEXT("RDProfile"),   w, TR_PROFILE_CHECKBOX    );
save_checked_state( BackupSection, TEXT("RDParam"),     w, TR_PARAM_CHECKBOX      );
save_checked_state( BackupSection, TEXT("RDAlarm"),     w, TR_ALARM_CHECKBOX      );
save_checked_state( BackupSection, TEXT("RDSureTrak"),  w, TR_SURETRAK_CHECKBOX   );

w = MyWin[DELETE_DIALOG_INDEX];
save_checked_state( BackupSection, TEXT("DDParams"),   w, DD_PARAMETERS_CHECKBOX );
save_checked_state( BackupSection, TEXT("DDProfiles"), w, DD_PROFILES_CHECKBOX   );

save_checked_state( BackupSection, TEXT("DDDelete"),   w, DD_DELETE_RADIOBUTTON );
save_checked_state( BackupSection, TEXT("DDRenumber"), w, DD_RENUMBER_RADIOBUTTON   );

if ( get_text( buf, w, DD_RENUMBER_EDITBOX, MAX_INTEGER_LEN) )
    put_ini_string( VisiTrakIniFile, BackupSection, TEXT("DDFirstNum"), buf );

}

/***********************************************************************
*                          FILL_MACHINE_LIST                           *
***********************************************************************/
void fill_machine_list( void )
{
COMPUTER_CLASS c;

MachList.empty();

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() )
        MachList.add_computer( c.name() );
    }
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
static void init( int cmd_show )
{
COMPUTER_CLASS c;
TCHAR        * cp;
WNDCLASS       wc;

names_startup();

c.startup();
fill_machine_list();
if ( *get_ini_string( VisiTrakIniFile, ConfigSection, TEXT("HaveSureTrak") ) == YChar )
    HaveSureTrakControl = TRUE;

get_backup_directory();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("bupres.ico") );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

cp = resource_string( MainInstance, MAIN_CAPTION_STRING );

MainWindow = CreateWindow(
    MyClassName,                       // window class name
    cp,                                // window title
    WS_CAPTION | WS_THICKFRAME | WS_SYSMENU,
    0, 0,                             // X,y
    282, 264,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

ChooseFunctionWindow = CreateDialog(
    MainInstance,
    TEXT( "CHOOSE_FUNCTION" ),
    MainWindow,
    (DLGPROC) choose_function_proc );

CurrentWindow = ChooseFunctionWindow;

MyWin[BACKUP_DIALOG_INDEX]      = CreateDialog( MainInstance, TEXT("BACKUP_DIALOG"),      MainWindow, (DLGPROC) backup_proc      );
MyWin[BACKUP_LIST_DIALOG_INDEX] = CreateDialog( MainInstance, TEXT("BACKUP_LIST_DIALOG"), MainWindow, (DLGPROC) backup_list_proc );
MyWin[RESTORE_DIALOG_INDEX]     = CreateDialog( MainInstance, TEXT("RESTORE_DIALOG"),     MainWindow, (DLGPROC) restore_proc     );
MyWin[DELETE_DIALOG_INDEX]      = CreateDialog( MainInstance, TEXT("DELETE_DATA"),        MainWindow, (DLGPROC) delete_data_proc );

restore_config();

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );

SetFocus( ChooseFunctionWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
save_config();
}

/***********************************************************************
*                         MY_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN my_previous_instance( void )
{
int i;
int si;
TCHAR * cp;

cp = resource_string( MainInstance, MAIN_CAPTION_STRING );
if ( is_previous_instance(MyClassName, cp) )
    return TRUE;

for ( i=0; i<DIALOG_COUNT; i++ )
    {
    si = BACKUP_DATA_STRING + i;
    cp = resource_string( MainInstance, si );
    if ( is_previous_instance(MyClassName, cp) )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
int     i;
BOOL    status;
MSG     msg;

InitCommonControls();

MainInstance = this_instance;

if ( my_previous_instance() )
    return 0;

init( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( ChooseFunctionWindow )
        status = IsDialogMessage( ChooseFunctionWindow, &msg );

    for ( i=0; i<DIALOG_COUNT; i++ )
        {
        if ( !status )
            {
            if ( MyWin[i] )
                status = IsDialogMessage( MyWin[i], &msg );
            }
        }

    if ( !status )
       {
       TranslateMessage(&msg);
       DispatchMessage(&msg);
       }
   }

shutdown();
return(msg.wParam);
}
